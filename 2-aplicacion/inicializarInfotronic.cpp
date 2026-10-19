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

// Display de 7 segmentos #########################################################
// segmentos - I4511_segmentos---------------------------------------------------------
Gpio g_bcdA(  Gpio::PORT0 , 23 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_bcdB(  Gpio::PORT0 , 22 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_bcdC(  Gpio::PORT0 , 21 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_bcdD(  Gpio::PORT0 , 20 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_dp(  Gpio::PORT0 ,   12 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::LOW );

Gpio *g_I4511_segmentos[] = { &g_bcdA,&g_bcdB,&g_bcdC,&g_bcdD,&g_dp};
I4511_Segmentos g_I4511Segmentos(g_I4511_segmentos);

// digitos - g_I4017_digitos ---------------------------------------------------------
Gpio g_rst(  Gpio::PORT0,  18 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_clk(  Gpio::PORT0 , 19 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );

Gpio *g_I4017_digitos[] = { &g_rst,&g_clk,};
I4017_Barrido g_I4017Barrido( g_I4017_digitos );

GrupoDeDigitos Grupos[]=
{
	{ 0 , 3 },
	{ 3 , 3 },
	{-1 , -1 }
};

Display7Segmentos g_Display( &g_I4511Segmentos , &g_I4017Barrido , Grupos );

// ##################################################################################
// TECLADO -----------------------------------------------------------------------
Gpio g_ret0 ( Gpio::PORT0 , 26 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
Gpio g_ret1 ( Gpio::PORT0 , 15 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );
Gpio g_ret2 ( Gpio::PORT0 ,  8 , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW );

Gpio g_scn0 ( Gpio::PORT0, 27, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );
Gpio g_scn1 ( Gpio::PORT0, 28, Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::HIGH );

Gpio *g_scn[] = {&g_scn0 , &g_scn1, nullptr};
Gpio *g_ret[] = {&g_ret0 , &g_ret1, &g_ret2,  nullptr};

Teclado g_Teclado( g_ret , g_scn );

//!<	ENTRADAS DIGITALES
Gpio g_in0( Gpio::PORT0 , 4 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW );
Gpio g_in1( Gpio::PORT0 , 7 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW);
Gpio g_in2( Gpio::PORT0 , 31 , Gpio::PULLUP ,  Gpio::INPUT , Gpio::LOW);

//!< GPIO ENTRADAS SALIDAS DE PROPOSITO GENERAL
Gpio g_out0( Gpio::PORT1 , 0 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_out1( Gpio::PORT1 , 1 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_out2( Gpio::PORT1 , 2 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );
Gpio g_out3( Gpio::PORT0 , 30 , Gpio::PUSHPULL ,  Gpio::OUTPUT , Gpio::HIGH );

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
	g_out3 = Gpio::OFF;

	SysTick_InstalarCallBack( Sheduller );
	SysTick_Inicializar(1);
}

