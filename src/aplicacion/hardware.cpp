/*******************************************************************************************************************************//**
 *
 * @file		hardware.cpp
 * @brief		Creacion de todos los objetos de hardware e inicializacion del sistema
 *
 * Los objetos son GLOBALES: se construyen antes de que arranque main(). Por eso sus
 * constructores no configuran perifericos que dependen del clock (SPI, UART, ADC):
 * eso se hace en InicializarHardware(), despues de InicializarClock().
 * (Gpio si configura el pin en el constructor; no depende del clock.)
 *
 **********************************************************************************************************************************/

#include "hardware.h"
#include "clock.h"
#include "systick.h"
#include "perifericotemporizado.h"

// ================================================================ OBJETOS (los "extern" de hardware.h apuntan aca)

// Matriz LED 16x32. LOAD (CS) es activo en bajo: SetPin() lo pone en 0 V.
Gpio		g_matrizLoad( MATRIZ_LOAD_PUERTO , MATRIZ_LOAD_PIN , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::LOW );
Spi			g_matrizSpi( MATRIZ_SPI , MATRIZ_CLK_PUERTO , MATRIZ_CLK_PIN , MATRIZ_DIN_PUERTO , MATRIZ_DIN_PIN );
Max7219		g_matriz( &g_matrizSpi , &g_matrizLoad );

// Joystick 1: un Adc por eje de la palanca
// Si un eje queda al reves:  Joystick g_joy1( &g_adcJoy1X , &g_adcJoy1Y , true , false );  (invierte X)
Adc			g_adcJoy1X( JOY1_X_CANAL , Adc::PROMEDIO );
Adc			g_adcJoy1Y( JOY1_Y_CANAL , Adc::PROMEDIO );
Joystick	g_joy1( &g_adcJoy1X , &g_adcJoy1Y );

// Joystick 1: boton de la palanca y los dos pulsadores
Pulsador	g_botonJoy1( JOY1_SW_PUERTO , JOY1_SW_PIN );
Pulsador	g_pulsDerJoy1( JOY1_DER_PUERTO , JOY1_DER_PIN );
Pulsador	g_pulsIzqJoy1( JOY1_IZQ_PUERTO , JOY1_IZQ_PIN );

// LEDs del kit (activos en alto)
Gpio		g_led0( LED0_PUERTO , LED0_PIN , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
Gpio		g_led1( LED1_PUERTO , LED1_PIN , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
Gpio		g_led2( LED2_PUERTO , LED2_PIN , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );

// PC
Uart		g_uartPC( UART_PC , UART_PC_TX_PUERTO , UART_PC_TX_PIN , UART_PC_RX_PUERTO , UART_PC_RX_PIN , UART_PC_BAUDIOS );

// ================================================================ TIEMPO

static volatile uint32_t s_ms = 0;

#define	PERIODO_PULSADORES_MS	10

// Se ejecuta cada 1 ms dentro de la interrupcion del SysTick. Tiene que ser CORTA.
static void TickDeSistema( void )
{
	s_ms++;

	Adc::Disparar();							// nueva conversion del joystick
	PerifericoTemporizado::AtenderTodos();		// descuenta los Timer

	if ( s_ms % PERIODO_PULSADORES_MS == 0 )	// antirrebote de los pulsadores
	{
		g_botonJoy1.Actualizar();
		g_pulsDerJoy1.Actualizar();
		g_pulsIzqJoy1.Actualizar();
	}
}

uint32_t Milisegundos( void )
{
	return s_ms;
}

void Esperar( uint32_t ms )
{
	uint32_t inicio = s_ms;
	while ( ( s_ms - inicio ) < ms )
	{
	}
}

// ================================================================ INICIALIZACION

bool InicializarHardware( void )
{
	// 1- Clock a 30 MHz: TODO lo demas depende de esto
	InicializarClock();

	// 2- ADC (antes de arrancar el SysTick, que lo dispara)
	Adc::Inicializar();

	// 3- Base de tiempo de 1 ms
	SysTick_InstalarCallBack( TickDeSistema );
	if ( !SysTick_Inicializar( 1 ) )
		return false;

	// 4- LEDs apagados
	g_led0 = Gpio::OFF;
	g_led1 = Gpio::OFF;
	g_led2 = Gpio::OFF;

	// 5- Matriz LED (si no esta conectada no pasa nada: el SPI manda igual)
	if ( !g_matrizSpi.Inicializar( FREQ_CLOCK , MATRIZ_VELOCIDAD_SPI , 0 ) )
		return false;
	if ( !g_matriz.Inicializar( 1 ) )		// brillo 1 de 15
		return false;

	// 6- Comunicacion con la PC
	if ( !g_uartPC.Inicializar() )
		return false;

	// 7- Joystick: esperar que se llene el promedio (16 muestras x 1 ms) y medir el centro
	Esperar( 100 );
	g_joy1.Calibrar();

	return true;
}
