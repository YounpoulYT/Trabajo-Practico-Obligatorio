/*******************************************************************************************************************************//**
 *
 * @file		adc.cpp
 * @brief		Driver del ADC del LPC845 (secuencia A, varios canales, promedio movil)
 * @date		9 nov. 2022
 * @author		Ing. Marcelo Trujillo
 *
 * Modificado para el TPO (consola arcade, oct. 2026):
 *  - Se habilitan los canales 3, 4, 5 y 6 (P0_23, P0_22, P0_21, P0_20) para los dos joysticks.
 *  - Se apagan los pull-ups internos de los pines analogicos.
 *  - La secuencia ya NO se relanza sola dentro de la interrupcion: se dispara desde afuera
 *    con Adc::Disparar() (por ejemplo cada 1 ms desde el SysTick). Asi el ADC no ocupa la CPU
 *    todo el tiempo y el juego corre fluido.
 *  - La interrupcion salta al terminar la secuencia COMPLETA y lee el registro DAT de cada canal.
 *  - Corregido el while de la calibracion (usaba &= en lugar de &).
 *  - CRUDO devuelve la ultima muestra del canal (antes dependia del filtro del primer objeto).
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Adc.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define	INT_SEQ_A				0		// bit SEQA_INTEN del registro INTEN
#define	FLAG_INTERRUPT_SEQ_A	28		// bit SEQA_INT del registro FLAGS

#define	CLOCK_SWM				7		// bit de SYSAHBCLKCTRL0
#define	CLOCK_IOCON				18		// bit de SYSAHBCLKCTRL0

#define	IOCON_MODE_SHIFT		3		// bits 4:3 de IOCON = pull-up / pull-down / nada
#define	IOCON_MODE_MASK			( 0x3 << IOCON_MODE_SHIFT )

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
struct canal
{
	uint16_t	Indice;		// proxima posicion a escribir en el buffer circular del canal
	bool		enable;		// true = el canal participa de la secuencia
	uint8_t		iocon;		// indice del pin dentro de IOCON->PIO[] (ver manual, tabla de IOCON)
};

// Cada canal del ADC esta atado a UN pin fijo del micro (no se puede elegir con la switch matrix).
// Lo que hay conectado a cada pin en el kit Infotronic sale del esquematico kit22025.pdf
struct canal CanalesHabilitados[] =
{
	{ 0 , false , 15 },	// canal 0  : P0_7
	{ 0 , false , 16 },	// canal 1  : P0_6  -> Ain0 del kit (POT1 / NTC segun el DIP S1). Reserva.
	{ 0 , false , 18 },	// canal 2  : P0_14 -> LCD-E, tiene pull-up de 10k a 5V. NO usar.
	{ 0 , true  , 25 },	// canal 3  : P0_23 -> BCDA, CN12-1 -> JOYSTICK 1, eje X
	{ 0 , true  , 26 },	// canal 4  : P0_22 -> BCDB, CN12-2 -> JOYSTICK 1, eje Y
	{ 0 , true  , 27 },	// canal 5  : P0_21 -> BCDC, CN12-3 -> JOYSTICK 2, eje X
	{ 0 , true  , 28 },	// canal 6  : P0_20 -> BCDD, CN12-4 -> JOYSTICK 2, eje Y
	{ 0 , false , 29 },	// canal 7  : P0_19 -> CK del 4017
	{ 0 , false , 30 },	// canal 8  : P0_18 -> RST del 4017
	{ 0 , false ,  0 },	// canal 9  : P0_17 -> RX1 / LOAD de la matriz
	{ 0 , false ,  1 },	// canal 10 : P0_13 -> LCD-D7, pull-up de 10k a 5V. NO usar.
	{ 0 , false ,  4 }	// canal 11 : P0_4
};

#define	CANTIDAD_CANALES	( sizeof( CanalesHabilitados ) / sizeof( CanalesHabilitados[0] ) )

// Ultimas TAMANIO_BUFFER_ADC muestras de cada canal.
// volatile: la escribe la interrupcion y la lee el programa principal.
static volatile uint32_t g_result[ CANTIDAD_CANALES ][ TAMANIO_BUFFER_ADC ] ;

Adc	*g_adc[ 4 ] = { nullptr , nullptr , nullptr ,nullptr };

uint8_t Adc::m_inicializar = 0 ;

static volatile uint32_t m_finished;

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
Adc::Adc(uint8_t canal ,  resultado_t TipoDeFiltro )
{
	m_callback = nullptr;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = TipoDeFiltro ;
	inicializar();
}

Adc::Adc(uint8_t canal ,  int32_t (*callback)(int32_t) )
{
	m_callback = callback;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = CALLBACK ;
	inicializar();
}

// El periferico es uno solo, asi que se configura una sola vez (con el primer objeto que se crea).
// Los canales que se convierten salen de la tabla CanalesHabilitados.
// IMPORTANTE: crear los objetos Adc DESPUES de Inicializar_PLL() (no como variables globales).
void Adc::inicializar(void)
{
	if ( m_inicializar )
		return;

	m_inicializar = true;

	// 1- Encender el ADC (PDRUNCFG es activo bajo) y darle clock
	SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;
	SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_ADC_MASK;

	// 2- Pasar los pines a funcion analogica
	SYSCON->SYSAHBCLKCTRL0 |= ( 1 << CLOCK_SWM ) | ( 1 << CLOCK_IOCON );

	for ( uint8_t i = 0 ; i < CANTIDAD_CANALES ; i++ )
	{
		if ( CanalesHabilitados[i].enable )
		{
			// Switch matrix: habilita la funcion fija ADC_i en su pin (registro activo bajo)
			SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_MASK( i );

			// IOCON: sin pull-up ni pull-down. Por defecto el pin arranca con pull-up,
			// que "tira" la tension hacia 3.3V y falsea la lectura del potenciometro.
			IOCON->PIO[ CanalesHabilitados[i].iocon ] &= ~IOCON_MODE_MASK;
		}
	}

	SYSCON->SYSAHBCLKCTRL0 &= ~( 1 << CLOCK_SWM );

	// 3- Autocalibracion (se hace una vez al arrancar)
	calibrateADC( );

	// 4- Clock del ADC: modo sincronico (usa el clock del sistema) dividido por (12 + 1)
	ADC0->CTRL &= ~(uint32_t)ADC_CTRL_ASYNMODE_MASK;
	set_clkdiv(12);
	ADC0->CTRL &= ~(uint32_t)ADC_CTRL_LPWRMODE_MASK;

	// 5- Secuencia A: que canales convierte y cuando interrumpe
	uint32_t canales = 0;
	for ( uint8_t i = 0 ; i < CANTIDAD_CANALES ; i++ )
	{
		if ( CanalesHabilitados[i].enable )
			canales |= ( 1 << i );
	}

	ADC0->SEQ_CTRL[0] = 0;									// secuencia deshabilitada mientras se configura
	ADC0->SEQ_CTRL[0] = ADC_SEQ_CTRL_CHANNELS( canales )	// canales que entran en la secuencia
					  | ADC_SEQ_CTRL_MODE_MASK;				// MODE = 1: interrumpe al terminar TODA la secuencia
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_SEQ_ENA_MASK;			// habilita la secuencia A

	// 6- Interrupcion de fin de secuencia A
	ADC0->INTEN |= ( 1 << INT_SEQ_A );
	g_adc[ 0 ] = this ;
	NVIC->ISER[0] = ( 1 << ISE_ADC_SEQA );	// ISER: escribir 1 habilita, los 0 no tocan nada
}

Adc::~Adc()
{
}

// Lanza UNA pasada por todos los canales de la secuencia A.
// Cada conversion tarda unos pocos microsegundos, asi que se puede llamar cada 1 ms sin problema.
void Adc::Disparar(void)
{
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_START_MASK;
}

void Adc::start_conversion(void)
{
	Disparar();
}

uint32_t Adc::finished(uint16_t *channel_0, uint16_t *channel_1)
{
	uint32_t ret = 0;

	NVIC->ICER[0] = ( 1 << ISE_ADC_SEQA );		// deshabilita SOLO la interrupcion del ADC
	if (m_finished)
	{
		m_finished = 0;
		ret = 1;
	}
	NVIC->ISER[0] = ( 1 << ISE_ADC_SEQA );		// y la vuelve a habilitar

	return ret;
}

void Adc::seqa_set_mode_end_of_conversion(void)
{
	ADC0->SEQ_CTRL[0] &= ~ADC_SEQ_CTRL_MODE_MASK;
}

void Adc::seqa_set_mode_end_of_sequence(void)
{
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_MODE_MASK;
}

void Adc::set_clkdiv(uint8_t div)
{
	ADC0->CTRL &= ~ADC_CTRL_CLKDIV_MASK;
	ADC0->CTRL |= div;
}

void Adc::seqa_set_channels(uint16_t channel)
{
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_CHANNELS( 1 << channel );
}

void Adc::seqa_enable_sequence(void)
{
	ADC0->SEQ_CTRL[0] |= ADC_SEQ_CTRL_SEQ_ENA_MASK;
}

void Adc::inten_enable_seqa(void)
{
	ADC0->INTEN |= (1 << INT_SEQ_A) ;
}

void Adc::setBurstMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<START);
	ADC0->SEQ_CTRL[0] &= ~(1<<SINGLESTEP);
	ADC0->SEQ_CTRL[0] |= (1<<BURST);
}

void Adc::setSingleSequenceMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<SINGLESTEP);
	ADC0->SEQ_CTRL[0] &= ~(1<<BURST);
	ADC0->SEQ_CTRL[0] |= (1<<START);
}

void Adc::setStepMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<BURST);
	ADC0->SEQ_CTRL[0] &= ~(1<<START);
	ADC0->SEQ_CTRL[0] |= (1<<SINGLESTEP);
}

// Se ejecuta cuando termino la secuencia completa (todos los canales habilitados).
// Cada canal tiene su propio registro de resultado: ADC0->DAT[canal].
void Adc::ADC_seqA_IRQHandler(void)
{
	for ( uint8_t i = 0 ; i < CANTIDAD_CANALES ; i++ )
	{
		if ( CanalesHabilitados[i].enable )
		{
			uint32_t dato = ADC0->DAT[i];
			uint16_t &k = CanalesHabilitados[i].Indice;

			// El resultado de 12 bits esta en los bits 15:4 del registro
			g_result[ i ][ k ] = ( dato & ADC_DAT_RESULT_MASK ) >> ADC_DAT_RESULT_SHIFT;

			k++;
			k %= TAMANIO_BUFFER_ADC;
		}
	}

	// En modo "fin de secuencia" el flag se borra escribiendo un 1 en SEQA_INT.
	// Se usa = y no |= : FLAGS se borra escribiendo 1, y con |= borrariamos tambien otros flags.
	ADC0->FLAGS = ( 1 << FLAG_INTERRUPT_SEQ_A );

	m_finished = 1;
}

void Adc::ADC_seqB_IRQHandler(void)
{
	return ;
}
void Adc::ADC_thcmp_IRQHandler(void)
{
	return ;
}
void Adc::ADC_ovr_IRQHandler(void)
{
	return ;
}

void Adc::calibrateADC(void){

	uint32_t temporal;
	uint32_t frecuencia = 0U;

	// a- Se debe obtener la frecuencia del ADC
	frecuencia = FREQ_CLOCK;
	temporal = ADC0->CTRL;

	// b- Se debe llevar la frecuencia del ADC a 500kHz
	ADC0->CTRL &= ~ADC_CTRL_CLKDIV_MASK;
	ADC0->CTRL |= ADC_CTRL_CLKDIV((frecuencia / 500000U) - 1U);

	/* Clear the LPWR bit. */
	ADC0->CTRL &= ~ADC_CTRL_LPWRMODE_MASK;

	/* Start ADC self-calibration. */
	ADC0->CTRL |= ADC_CTRL_CALMODE_MASK;

	// El hardware pone CALMODE en 0 cuando termina.
	// (Antes decia "&=", que ESCRIBIA el registro en cada vuelta en lugar de solo leerlo)
	while( ADC0->CTRL & ADC_CTRL_CALMODE_MASK )
	{
	}

	/* Restore the contents of the ADC CTRL register. */
	ADC0->CTRL = temporal;
}

void Adc::setFiltro (uint32_t tipo) {

	switch (tipo) {

		case PROMEDIO:
			m_TipoDeFiltro = PROMEDIO;
			break;

		case CRUDO:
		default:
			m_TipoDeFiltro = CRUDO;
			break;
	}
}

void ADC_SEQA_IRQHandler(void)
{
	if ( g_adc[0] )
		g_adc[0]->ADC_seqA_IRQHandler( );
}

void ADC_SEQB_IRQHandler(void)
{
	if ( g_adc[1] )
		g_adc[1]->ADC_seqB_IRQHandler( );

}
void ADC_THCMP_IRQHandler(void)
{
	if ( g_adc[2] )
		g_adc[2]->ADC_thcmp_IRQHandler( );

}
void ADC_OVR_IRQHandler(void)
{
	if ( g_adc[3] )
		g_adc[3]->ADC_ovr_IRQHandler( );

}

// Devuelve el valor del canal: 0 (0V) a 4095 (3.3V)
int32_t Adc::getResultado(void) {

	uint32_t resultadoParcial = 0;

	if ( m_NumeroCanal >= CANTIDAD_CANALES )
		return 0;

	switch ( m_TipoDeFiltro )
	{
		case CRUDO :
		{
			// La ultima muestra guardada esta una posicion antes del indice de escritura
			uint16_t ultimo = ( CanalesHabilitados[m_NumeroCanal].Indice + TAMANIO_BUFFER_ADC - 1 ) % TAMANIO_BUFFER_ADC;
			resultadoParcial = g_result[m_NumeroCanal][ultimo];
			break;
		}
		case PROMEDIO:
			resultadoParcial = promedio( );
			break;
		case CALLBACK:
			resultadoParcial = promedio( );
			resultadoParcial = m_callback ( resultadoParcial );
			break;
	}

	return resultadoParcial;
}

// Promedio movil de las ultimas TAMANIO_BUFFER_ADC muestras: filtra el ruido del potenciometro
uint32_t Adc::promedio( void )
{
	uint32_t resultadoParcial = 0;
	for (uint32_t i=0 ; i < TAMANIO_BUFFER_ADC ; i++)
		resultadoParcial += g_result[m_NumeroCanal][i];
	resultadoParcial /= TAMANIO_BUFFER_ADC;
	return resultadoParcial ;
}
