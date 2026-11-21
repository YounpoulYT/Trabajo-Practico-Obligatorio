/*******************************************************************************************************************************//**
 *
 * @file		main.cpp
 * @brief		PRUEBA DEL JOYSTICK 1 con la matriz de 16x32 y los 3 LEDs del kit
 *
 *  - La palanca mueve un punto por toda la matriz (16 filas x 32 columnas).
 *  - Boton de la palanca (SW, P0_19): el punto vuelve al centro de la matriz.
 *  - Los LEDs del kit muestran los botones mientras estan apretados:
 *
 *        LED0 = pulsador IZQUIERDO (P0_23)
 *        LED1 = boton de la palanca (P0_19)
 *        LED2 = pulsador DERECHO (P0_22)
 *
 *  - Cada 250 ms manda a la PC (UART0, 9600) la linea  #JOY,x1,y1,x2,y2$  con los valores crudos (0..4095).
 *    El joystick 2 todavia no esta conectado: en su lugar se manda 2048 (palanca en el centro).
 *    Se ve con la terminal de MCUXpresso.
 *
 * Las conexiones estan en pines.h. Los objetos (g_joy1, g_matriz, g_botonJoy1, ...) estan en hardware.cpp.
 *
 **********************************************************************************************************************************/

#include "hardware.h"

// ============================================================ MATRIZ

// Mueve el punto un lugar segun la direccion, sin salirse de la matriz
static void MoverPunto( Joystick::direccion_t direccion , uint8_t &fila , uint8_t &columna )
{
	if ( direccion == Joystick::ARRIBA && fila > 0 )
		fila--;
	if ( direccion == Joystick::ABAJO && fila < MATRIZ_FILAS - 1 )
		fila++;
	if ( direccion == Joystick::IZQUIERDA && columna > 0 )
		columna--;
	if ( direccion == Joystick::DERECHA && columna < MATRIZ_COLUMNAS - 1 )
		columna++;
}

// ============================================================ PC

// Agrega un numero en texto al final de "texto" (sin sprintf, que ocupa mucha memoria)
static void AgregarNumero( char *texto , uint32_t &largo , int32_t numero )
{
	char cifras[ 12 ];
	uint8_t n = 0;

	if ( numero < 0 )
	{
		texto[ largo++ ] = '-';
		numero = -numero;
	}
	do
	{
		cifras[ n++ ] = '0' + ( numero % 10 );
		numero /= 10;
	} while ( numero > 0 );

	while ( n > 0 )
		texto[ largo++ ] = cifras[ --n ];
}

// Arma y envia:  #JOY,x1,y1,x2,y2$
static void EnviarJoysticks( void )
{
	char linea[ 48 ];
	uint32_t largo = 0;
	const char *encabezado = "#JOY,";

	while ( *encabezado )
		linea[ largo++ ] = *encabezado++;

	AgregarNumero( linea , largo , g_adcJoy1X.GetResultado() );	linea[ largo++ ] = ',';
	AgregarNumero( linea , largo , g_adcJoy1Y.GetResultado() );	linea[ largo++ ] = ',';
	AgregarNumero( linea , largo , Joystick::CENTRO_TEORICO );	linea[ largo++ ] = ',';		// joystick 2: no conectado
	AgregarNumero( linea , largo , Joystick::CENTRO_TEORICO );
	linea[ largo++ ] = '$';
	linea[ largo++ ] = '\r';
	linea[ largo++ ] = '\n';

	g_uartPC.Transmitir( (const uint8_t *) linea , largo );
}

// ============================================================ MAIN

int main( void )
{
	if ( !InicializarHardware() )
	{
		while ( 1 )			// algo fallo al iniciar
		{
		}
	}

	// Punto que mueve el joystick 1: arranca en el centro de la matriz
	const uint8_t FILA_CENTRO    = MATRIZ_FILAS / 2;
	const uint8_t COLUMNA_CENTRO = MATRIZ_COLUMNAS / 2;

	uint8_t fila = FILA_CENTRO;
	uint8_t columna = COLUMNA_CENTRO;

	const uint32_t PERIODO_LEDS   = 20;		// ms
	const uint32_t PERIODO_MATRIZ = 120;	// ms -> velocidad del punto
	const uint32_t PERIODO_PC     = 250;	// ms

	uint32_t tLeds   = Milisegundos();
	uint32_t tMatriz = Milisegundos();
	uint32_t tPC     = Milisegundos();

	while ( 1 )
	{
		// ---------------- Boton de la palanca (el antirrebote lo hace el SysTick) ----------------
		if ( g_botonJoy1.FuePresionado() )
		{
			fila = FILA_CENTRO;
			columna = COLUMNA_CENTRO;
		}

		// ---------------- LEDs: muestran los botones ----------------
		if ( Milisegundos() - tLeds >= PERIODO_LEDS )
		{
			tLeds += PERIODO_LEDS;

			g_led0 = g_pulsIzqJoy1.EstaPresionado();
			g_led1 = g_botonJoy1.EstaPresionado();
			g_led2 = g_pulsDerJoy1.EstaPresionado();
		}

		// ---------------- Matriz ----------------
		if ( Milisegundos() - tMatriz >= PERIODO_MATRIZ )
		{
			tMatriz += PERIODO_MATRIZ;

			MoverPunto( g_joy1.GetDireccion() , fila , columna );

			g_matriz.Limpiar();
			g_matriz.SetPixel( fila , columna );
			g_matriz.Actualizar();
		}

		// ---------------- PC ----------------
		if ( Milisegundos() - tPC >= PERIODO_PC )
		{
			tPC += PERIODO_PC;
			EnviarJoysticks();
		}
	}
}
