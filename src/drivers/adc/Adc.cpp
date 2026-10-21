/*******************************************************************************************************************************//**
 *
 * @file		Adc.cpp
 * @brief		Conversor analogico-digital (ADC) del LPC845
 * @author		Ing. Marcelo Trujillo (reescrito para el TPO)
 *
 * Diferencias con el driver de la catedra:
 *  - Los canales se habilitan solos segun los objetos que se crean (antes habia una tabla fija).
 *  - El constructor no toca el hardware; se configura con Adc::Inicializar().
 *  - Se convierte una secuencia completa por disparo (cada 1 ms) y hay UNA interrupcion al final,
 *    en vez de relanzar el ADC sin parar dentro de la interrupcion.
 *  - Se apaga el pull-up interno de los pines analogicos.
 *  - Corregido el while de la calibracion (usaba &= en lugar de &).
 *
 **********************************************************************************************************************************/

#include "Adc.h"
#include "Gpio.h"

#define	CLOCK_SWM				7			// bits de SYSAHBCLKCTRL0
#define	CLOCK_IOCON				18
#define	NVIC_ADC_SEQA			16			// numero de interrupcion del ADC (secuencia A)
#define	INTEN_SEQA				0			// bit de INTEN
#define	FLAGS_SEQA_INT			28			// bit de FLAGS
#define	IOCON_MODE_MASK			( 0x3 << 3 )

#define	ADC_DIVISOR				12			// clock del ADC = 30 MHz / (12 + 1) = 2,3 MHz
#define	FREQ_CALIBRACION		500000UL	// la calibracion se hace a 500 kHz

// Pin de cada canal (todos estan en el puerto 0). Es fijo, viene del hardware del LPC845.
static const uint8_t PIN_DEL_CANAL[ Adc::CANTIDAD_CANALES ] =
{
	7 , 6 , 14 , 23 , 22 , 21 , 20 , 19 , 18 , 17 , 13 , 4		// canal 0 .. canal 11
};

uint16_t			Adc::s_canalesUsados = 0;
volatile uint16_t	Adc::s_muestras[ CANTIDAD_CANALES ][ MUESTRAS ];
volatile uint8_t	Adc::s_indice[ CANTIDAD_CANALES ];

Adc::Adc( uint8_t canal , filtro_t filtro )
{
	m_canal = ( canal < CANTIDAD_CANALES ) ? canal : 0;
	m_filtro = filtro;
	s_canalesUsados |= ( 1 << m_canal );		// queda anotado para Inicializar()
}

void Adc::Inicializar( void )
{
	// 1- Encender el ADC (PDRUNCFG es activo bajo) y darle clock
	SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_ADC_MASK | ( 1 << CLOCK_SWM ) | ( 1 << CLOCK_IOCON );

	// 2- Poner en modo analogico los pines de los canales usados
	for ( uint8_t c = 0 ; c < CANTIDAD_CANALES ; c++ )
	{
		if ( s_canalesUsados & ( 1 << c ) )
		{
			// Switch matrix: habilita la funcion fija ADC_c en su pin (activo bajo)
			SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_MASK( c );

			// IOCON: sin pull-up. Por defecto el pin arranca con pull-up y eso falsea la lectura.
			IOCON->PIO[ Gpio::IndiceIOCON( Gpio::PORT0 , PIN_DEL_CANAL[ c ] ) ] &= ~IOCON_MODE_MASK;
		}
	}
	SYSCON->SYSAHBCLKCTRL0 &= ~( 1 << CLOCK_SWM );

	// 3- Autocalibracion
	Calibrar();

	// 4- Clock del ADC: modo sincronico (clock del sistema / (ADC_DIVISOR + 1)), sin bajo consumo
	ADC0->CTRL = ADC_CTRL_CLKDIV( ADC_DIVISOR );

	// 5- Secuencia A: que canales convierte, y que interrumpa al terminar TODA la secuencia
	ADC0->SEQ_CTRL[0] = 0;
	ADC0->SEQ_CTRL[0] = ADC_SEQ_CTRL_CHANNELS( s_canalesUsados ) | ADC_SEQ_CTRL_MODE_MASK;
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_SEQ_ENA_MASK;

	// 6- Interrupcion
	ADC0->INTEN = ( 1 << INTEN_SEQA );
	NVIC->ISER[0] = ( 1 << NVIC_ADC_SEQA );
}

void Adc::Calibrar( void )
{
	// La calibracion tiene que hacerse con el ADC a 500 kHz
	ADC0->CTRL = ADC_CTRL_CLKDIV( ( FREQ_CLOCK / FREQ_CALIBRACION ) - 1 ) | ADC_CTRL_CALMODE_MASK;

	// El hardware pone CALMODE en 0 cuando termina
	while ( ADC0->CTRL & ADC_CTRL_CALMODE_MASK )
	{
	}
}

void Adc::Disparar( void )
{
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_START_MASK;
}

// Se ejecuta al terminar la secuencia. Cada canal tiene su registro de resultado: ADC0->DAT[canal].
void Adc::ManejarInterrupcion( void )
{
	for ( uint8_t c = 0 ; c < CANTIDAD_CANALES ; c++ )
	{
		if ( s_canalesUsados & ( 1 << c ) )
		{
			// El resultado (12 bits) esta en los bits 15:4 del registro
			uint16_t valor = ( ADC0->DAT[ c ] & ADC_DAT_RESULT_MASK ) >> ADC_DAT_RESULT_SHIFT;

			uint8_t i = s_indice[ c ];
			s_muestras[ c ][ i ] = valor;
			s_indice[ c ] = ( i + 1 ) % MUESTRAS;		// buffer circular
		}
	}

	// Borrar el flag: en FLAGS escribir un 1 borra. Con "=" se borra solo este (con "|=" se borrarian todos).
	ADC0->FLAGS = ( 1 << FLAGS_SEQA_INT );
}

int32_t Adc::GetResultado( void ) const
{
	if ( m_filtro == CRUDO )
	{
		// La ultima muestra esta una posicion antes del indice de escritura
		uint8_t ultima = ( s_indice[ m_canal ] + MUESTRAS - 1 ) % MUESTRAS;
		return s_muestras[ m_canal ][ ultima ];
	}

	// Promedio movil: filtra el ruido del potenciometro
	uint32_t suma = 0;
	for ( uint8_t i = 0 ; i < MUESTRAS ; i++ )
		suma += s_muestras[ m_canal ][ i ];
	return suma / MUESTRAS;
}

void ADC_SEQA_IRQHandler( void )
{
	Adc::ManejarInterrupcion();
}
