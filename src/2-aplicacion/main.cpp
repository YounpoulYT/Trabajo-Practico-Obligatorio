/*******************************************************************************************************************************//**
 *
 * @file		main.cpp
 * @brief		PRUEBA DE JOYSTICKS sobre la matriz de 8x32
 * @date		21 oct. 2026
 *
 * Que hace:
 *  - MODO PUNTOS: cada joystick mueve un punto por la matriz (J1 arranca a la izquierda, J2 a la derecha).
 *  - MODO BARRAS: muestra 4 barras (J1-X, J1-Y, J2-X, J2-Y), una por modulo 8x8.
 *    Palanca suelta = barra por la mitad. Sirve para verificar el cableado y ver los valores crudos.
 *  - Pulsador en P0_26: cambia entre MODO PUNTOS y MODO BARRAS.
 *  - Pulsador en P0_15: recalibra el centro de los joysticks y vuelve los puntos al inicio.
 *
 * Conexiones:
 *  - Matriz:     CLK = P0_16, DIN = P0_27, LOAD = P0_17
 *  - Joystick 1: VRx = P0_23 (CN12-1), VRy = P0_22 (CN12-2)
 *  - Joystick 2: VRx = P0_21 (CN12-3), VRy = P0_20 (CN12-4)
 *  - Joysticks:  "+5V" del modulo a 3,3 V (!!), GND a GND
 *  - Pulsadores: P0_26 y P0_15 a GND
 *
 * La version anterior de este main (las 4 caritas con efecto de brillo) queda en el historial de Git.
 *
 **********************************************************************************************************************************/

#include "dr_pll.h"
#include "systick.h"
#include "Gpio.h"
#include "Adc.h"
#include "1-modulos/14-SPI/SPI.h"
#include "1-modulos/MAX7219.h"
#include "1-modulos/15-joystick/Joystick.h"
#include "1-modulos/15-joystick/Pulsador.h"

// ============================================================
// TIEMPO
// ============================================================

// Milisegundos desde que arranco el programa. Lo incrementa el SysTick.
static volatile uint32_t g_ms = 0;

// Se ejecuta cada 1 ms dentro de la interrupcion del SysTick.
// Tiene que ser CORTA: solo cuenta el tiempo y lanza una conversion del ADC.
static void TickDeSistema( void )
{
	g_ms++;
	Adc::Disparar();
}

static void Esperar( uint32_t ms )
{
	uint32_t inicio = g_ms;
	while ( ( g_ms - inicio ) < ms )
	{
	}
}

static void Error( void )
{
	while ( 1 )
	{
	}
}

// ============================================================
// DIBUJO
// ============================================================

static const uint8_t FILAS = 8;
static const uint8_t COLUMNAS = 32;

// Mueve una posicion un paso en la direccion indicada, sin salirse de la matriz
static void Mover( Joystick::direccion_t direccion , int8_t &fila , int8_t &columna )
{
	switch ( direccion )
	{
		case Joystick::ARRIBA:		if ( fila > 0 )					fila--;		break;
		case Joystick::ABAJO:		if ( fila < FILAS - 1 )			fila++;		break;
		case Joystick::IZQUIERDA:	if ( columna > 0 )				columna--;	break;
		case Joystick::DERECHA:		if ( columna < COLUMNAS - 1 )	columna++;	break;
		case Joystick::CENTRO:
		default:
			break;
	}
}

// Dibuja una barra vertical en un modulo 8x8 (modulo 0..3).
// valor: 0..4095 (cuentas del ADC) -> altura 0..8 LEDs
static void DibujarBarra( Max7219 &matriz , uint8_t modulo , int32_t valor )
{
	int32_t altura = ( valor * 9 ) / 4096;
	if ( altura > FILAS )
		altura = FILAS;

	uint8_t columnaInicial = modulo * 8;

	for ( uint8_t fila = 0 ; fila < FILAS ; fila++ )
	{
		bool encendido = ( fila >= FILAS - altura );		// se llena desde abajo (fila 7)

		// columnas 1 a 6 del modulo: deja un espacio entre barras
		for ( uint8_t c = 1 ; c <= 6 ; c++ )
			matriz.SetPixel( fila , columnaInicial + c , encendido );
	}
}

// ============================================================
// MAIN
// ============================================================

int main( void )
{
	Inicializar_PLL();

	SysTick_InstalarCallBack( TickDeSistema );
	SysTick_Inicializar( 1 );

	// --------------------------------------------------------
	// MATRIZ 8x32 (4 x MAX7219)
	// --------------------------------------------------------
	Gpio loadMatriz( Gpio::PORT0 , 17 , Gpio::PUSHPULL , Gpio::OUTPUT , Gpio::LOW );
	Spi spiMatriz( 0 , Gpio::PORT0 , 16 , Gpio::PORT0 , 27 );		// SCK = P0_16, MOSI = P0_27

	if ( !spiMatriz.Inicializar( 24000000UL , 500000UL , 0 ) )
		Error();

	Max7219 matriz( &spiMatriz , &loadMatriz );
	if ( !matriz.Inicializar( 1 ) )
		Error();

	// --------------------------------------------------------
	// JOYSTICKS: un objeto Adc por eje (el numero es el CANAL del ADC)
	// Se crean aca adentro, despues de Inicializar_PLL(), y no como globales.
	// --------------------------------------------------------
	Adc j1x( 3 , Adc::PROMEDIO );		// P0_23
	Adc j1y( 4 , Adc::PROMEDIO );		// P0_22
	Adc j2x( 5 , Adc::PROMEDIO );		// P0_21
	Adc j2y( 6 , Adc::PROMEDIO );		// P0_20

	Joystick joy1( &j1x , &j1y );
	Joystick joy2( &j2x , &j2y );

	// --------------------------------------------------------
	// PULSADORES
	// --------------------------------------------------------
	Pulsador botonModo( Gpio::PORT0 , 26 );
	Pulsador botonCalibrar( Gpio::PORT0 , 15 );

	// Esperamos a que se llene el promedio (16 muestras x 1 ms) y tomamos el centro.
	// NO tocar los joysticks mientras arranca.
	Esperar( 100 );
	joy1.Calibrar();
	joy2.Calibrar();

	// --------------------------------------------------------
	// ESTADO DE LA PRUEBA
	// --------------------------------------------------------
	enum modo_t { MODO_PUNTOS , MODO_BARRAS };
	modo_t modo = MODO_PUNTOS;

	int8_t fila1 = 3 , columna1 = 3;		// punto del jugador 1
	int8_t fila2 = 4 , columna2 = 28;		// punto del jugador 2

	// Cada tarea corre cada cierto tiempo. Nada se queda "esperando":
	// el while(1) pasa muchas veces por segundo y cada tarea mira si ya le toca.
	const uint32_t PERIODO_BOTONES    = 10;		// ms
	const uint32_t PERIODO_MOVIMIENTO = 120;	// ms  -> velocidad del punto
	const uint32_t PERIODO_PANTALLA   = 40;		// ms  -> 25 cuadros por segundo

	uint32_t tBotones = g_ms , tMovimiento = g_ms , tPantalla = g_ms;

	while ( 1 )
	{
		// ---------------- Botones ----------------
		if ( g_ms - tBotones >= PERIODO_BOTONES )
		{
			tBotones += PERIODO_BOTONES;

			botonModo.Actualizar();
			botonCalibrar.Actualizar();

			if ( botonModo.FuePresionado() )
				modo = ( modo == MODO_PUNTOS ) ? MODO_BARRAS : MODO_PUNTOS;

			if ( botonCalibrar.FuePresionado() )
			{
				joy1.Calibrar();
				joy2.Calibrar();
				fila1 = 3; columna1 = 3;
				fila2 = 4; columna2 = 28;
			}
		}

		// ---------------- Movimiento ----------------
		if ( g_ms - tMovimiento >= PERIODO_MOVIMIENTO )
		{
			tMovimiento += PERIODO_MOVIMIENTO;

			Mover( joy1.GetDireccion() , fila1 , columna1 );
			Mover( joy2.GetDireccion() , fila2 , columna2 );
		}

		// ---------------- Pantalla ----------------
		if ( g_ms - tPantalla >= PERIODO_PANTALLA )
		{
			tPantalla += PERIODO_PANTALLA;

			matriz.Limpiar();

			if ( modo == MODO_PUNTOS )
			{
				matriz.SetPixel( fila1 , columna1 );
				matriz.SetPixel( fila2 , columna2 );
			}
			else
			{
				DibujarBarra( matriz , 0 , j1x.getResultado() );
				DibujarBarra( matriz , 1 , j1y.getResultado() );
				DibujarBarra( matriz , 2 , j2x.getResultado() );
				DibujarBarra( matriz , 3 , j2y.getResultado() );
			}

			if ( !matriz.Actualizar() )
				Error();
		}
	}
}
