/*******************************************************************************************************************************//**
 *
 * @file		inicializar.cpp
 * @brief		Descripcion del modulo
 * @date		5 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/

#include "inicializarInfotronic.h"
#include "aplicacion.h"
#include "AP_Timers.h"

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/
// Termistor a GND
static const int32_t Tabla_NTC[] =
{ 971,956,938,917,891,862,828,790,749,704,657,609,560,512,464,419,381,337,300,267,237,210,186,165,146,130,115,102,91,81,72,64,58,0};

/**********************************************************************************
 *** OBJETOS GLOBALES PUBLICOS
 **********************************************************************************/

// Display de 7 segmentos y teclado del kit: SE SACARON.
// Usaban P0_18..P0_23, P0_12 (display) y P0_26, P0_27, P0_28, P0_15, P0_8 (teclado),
// que ahora son de la matriz y de los joysticks (ver CONEXIONES.txt).

// MATRIZ LED 16x32 ##################################################################
// LOAD (CS) es activo en bajo: SetPin() lo pone en 0 V.
Gpio g_loadMatriz( Gpio::PORT0 , 17 , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::LOW );		// CS  - cable amarillo
Spi g_spiMatriz( 0 ,							// SPI0
		Gpio::PORT0 , 16 ,						// CLK - cable naranja
		Gpio::PORT0 , 18 );						// DIN - cable verde
Max7219 g_matriz( &g_spiMatriz , &g_loadMatriz );

// JOYSTICK 1 ########################################################################
// Palanca: un Adc por eje. Los canales tienen pin fijo (canal 5 = P0_21, canal 6 = P0_20)
// y se habilitan en la tabla CanalesHabilitados de Adc.cpp.
// Los dos ultimos parametros invierten X e Y (true = invertido).
// (Antes X estaba invertido, pero lo que estaba espejado era la matriz, no el joystick.)
Adc g_adcJoy1X( 5 , Adc::PROMEDIO );			// P0_21 - VRx - cable gris
Adc g_adcJoy1Y( 6 , Adc::PROMEDIO );			// P0_20 - VRy - cable azul
Joystick g_joy1( &g_adcJoy1X , &g_adcJoy1Y , false , true );

// Botones: a GND al apretar, con el pull-up interno. get() = 1 mientras esta apretado.
// Antirrebote: 20 lecturas iguales del SysTick (20 ms).
// Si el boton es un MODULO que da 3,3 V al apretar (en vez de 0 V), cambiar
//   Gpio::PULLUP , Gpio::LOW   por   Gpio::PULLDOWN , Gpio::HIGH
DigitalInputs g_swJoy1 ( Gpio::PORT0 , 19 , Gpio::PULLUP , Gpio::LOW , 20 );	// boton de la palanca - cable blanco
DigitalInputs g_derJoy1( Gpio::PORT0 , 26 , Gpio::PULLUP , Gpio::LOW , 20 );	// pulsador derecho   - cable naranja
DigitalInputs g_izqJoy1( Gpio::PORT0 , 27 , Gpio::PULLUP , Gpio::LOW , 20 );	// pulsador izquierdo - cable verde
// (Los pulsadores se pasaron de P0_22/P0_23 a P0_26/P0_27 para dejar libres los
//  pines con ADC P0_22 y P0_23 para los ejes del joystick 2.)

// JOYSTICK 2 ########################################################################
// Igual que el 1. Canal 3 = P0_23 y canal 4 = P0_22 (habilitados en la tabla de Adc.cpp).
// Se supone montado igual que el 1 (Y invertido). Si algun eje queda al reves, se cambia aca.
Adc g_adcJoy2X( 3 , Adc::PROMEDIO );			// P0_23 - VRx - cable gris
Adc g_adcJoy2Y( 4 , Adc::PROMEDIO );			// P0_22 - VRy - cable azul
Joystick g_joy2( &g_adcJoy2X , &g_adcJoy2Y , false , true );

DigitalInputs g_swJoy2 ( Gpio::PORT0 , 28 , Gpio::PULLUP , Gpio::LOW , 20 );	// boton de la palanca - cable blanco
DigitalInputs g_derJoy2( Gpio::PORT0 , 29 , Gpio::PULLUP , Gpio::LOW , 20 );	// pulsador derecho   - cable naranja
DigitalInputs g_izqJoy2( Gpio::PORT0 , 30 , Gpio::PULLUP , Gpio::LOW , 20 );	// pulsador izquierdo - cable verde

// Solo se usa para esperar al arrancar (ver InicializarInfotronic)
static Timer t_Arranque;

// ##################################################################################
//!<	ENTRADAS DIGITALES
Gpio g_in0( Gpio::PORT0 , 4 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW );
Gpio g_in1( Gpio::PORT0 , 7 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW);
Gpio g_in2( Gpio::PORT0 , 31 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW);

//!< GPIO ENTRADAS SALIDAS DE PROPOSITO GENERAL
Gpio g_out0( Gpio::PORT1 , 0 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_out1( Gpio::PORT1 , 1 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_out2( Gpio::PORT1 , 2 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
// g_out3 (P0_30) se saco: ese pin ahora es el pulsador izquierdo del joystick 2.

//!< ADC
Adc g_canal1( 1 , TablaNTC );
// terminal MCUXpresso
//UART_Init( 0U, 16U, 0U, 17U, USART1,
Uart uart0(
		0 , 				// PortTx
        25, 				// Pin Tx
		0 ,					// PortRx
		24, 				// Pin Rx
		0 ,					// uart seleccionada
		9600 ,				// baudios
		Uart::ocho_bits ,	// bits de datos
		Uart::NoParidad ,	// tipo de paridad
		64,					// tamaño de buffer de Rx
		64 );				// tamaño de buffer de Tx

int32_t TablaNTC( int32_t dato)
{
	uint8_t j;
	dato /= 4;// lo paso a 10 bits

	for(j = 0 ; j < 33; j++)
	{
		if( dato > Tabla_NTC[j] )
			break;
	}

	dato =   ( ( ( Tabla_NTC[j-1] - dato ) * 50000l ) / (Tabla_NTC[j-1] - Tabla_NTC[j]) ) / 10000 ;
	dato = dato + 5 * (j - 1) - 40;

	return dato;
}

void InicializarInfotronic ( void )
{
	Inicializar_PLL( );
	g_out0 = Gpio::OFF;
	g_out1 = Gpio::OFF;
	g_out2 = Gpio::OFF;

	SysTick_InstalarCallBack( Sheduller );
	SysTick_Inicializar(1);

	// Matriz LED: primero el SPI (modo 0) y despues los MAX7219
	if ( !g_spiMatriz.Inicializar( 24000000UL , 500000UL , 0 ) )
		while ( 1 ) ;
	if ( !g_matriz.Inicializar( 1 ) )			// brillo 1 de 15
		while ( 1 ) ;

	// Joystick: esperar que el ADC llene el promedio y medir el centro.
	// NO tocar la palanca mientras arranca.
	t_Arranque = 2;								// 2 decimas de segundo
	while ( !t_Arranque ) ;
	g_joy1.Calibrar();
	g_joy2.Calibrar();
}

