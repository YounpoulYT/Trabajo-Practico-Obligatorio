/*******************************************************************************************************************************//**
 *
 * @file		Aplicacion.cpp
 * @brief		PRUEBA DEL JOYSTICK 1 con la matriz de 16x32 y los LEDs de la placa
 * @date		12 nov. 2026
 * @author		Ing. Marcelo Trujillo (base) - Juan Pablo Klymenko Campos
 *
 *  - La palanca mueve un punto por toda la matriz (16 filas x 32 columnas).
 *  - Boton de la palanca (SW, P0_19): el punto vuelve al centro de la matriz.
 *  - Mientras se aprieta un pulsador se prende una columna entera de la matriz:
 *        pulsador IZQUIERDO -> columna 0 (borde izquierdo)
 *        pulsador DERECHO   -> columna 31 (borde derecho)
 *        boton de la palanca -> fila 0 (borde de arriba), ademas de llevar el punto al centro
 *  - Los LEDs (g_out0..2 = P1_0..P1_2) tambien muestran los botones:
 *        g_out0 = pulsador IZQUIERDO   g_out1 = boton de la palanca   g_out2 = pulsador DERECHO
 *  - Cada 2 decimas manda a la PC (uart0, 9600, por el USB de la placa) la linea:
 *
 *        #JOY,x1,y1,sw1,izq1,der1,x2,y2,sw2,izq2,der2$
 *
 *    x , y         : palanca, 0..4095 (~2048 suelta). Ya vienen con los ejes corregidos:
 *                    x crece hacia la DERECHA e y crece hacia ARRIBA.
 *    sw, izq, der  : botones, 1 = apretado, 0 = suelto.
 *    El 1 es el joystick 1 y el 2 el joystick 2.
 *
 * El ejemplo del control de temperatura que habia aca se saco porque usaba el display y el
 * teclado del kit, que ya no estan (sus pines ahora son de la matriz y los joysticks).
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "inicializarInfotronic.h"
#include "aplicacion.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/
#define		FILA_CENTRO			( MATRIZ_FILAS / 2 )
#define		COLUMNA_CENTRO		( MATRIZ_COLUMNAS / 2 )
#define		PERIODO_MATRIZ		1		// decimas de segundo: velocidad del punto
#define		PERIODO_PC			2		// decimas de segundo

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** OBJETOS GLOBALES PUBLICOS
 *********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
Timer t_Matriz , t_PC;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES DEL MODULO
 **********************************************************************************************************************************/
static uint8_t s_fila = FILA_CENTRO;			// donde esta el punto
static uint8_t s_columna = COLUMNA_CENTRO;
static uint8_t s_swAnterior = 0;				// para detectar cuando se APRIETA el SW

/***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

// Mueve el punto un lugar segun la direccion, sin salirse de la matriz
static void MoverPunto ( Joystick::direccion_t direccion )
{
	if ( direccion == Joystick::ARRIBA && s_fila > 0 )
		s_fila--;
	if ( direccion == Joystick::ABAJO && s_fila < MATRIZ_FILAS - 1 )
		s_fila++;
	if ( direccion == Joystick::IZQUIERDA && s_columna > 0 )
		s_columna--;
	if ( direccion == Joystick::DERECHA && s_columna < MATRIZ_COLUMNAS - 1 )
		s_columna++;
}

// Prende una columna entera de la matriz (las 16 filas)
static void DibujarColumna ( uint8_t columna )
{
	for ( uint8_t fila = 0 ; fila < MATRIZ_FILAS ; fila++ )
		g_matriz.SetPixel( fila , columna );
}

// Prende una fila entera de la matriz (las 32 columnas)
static void DibujarFila ( uint8_t fila )
{
	for ( uint8_t columna = 0 ; columna < MATRIZ_COLUMNAS ; columna++ )
		g_matriz.SetPixel( fila , columna );
}

// Agrega un numero en texto al final de "texto" (sin sprintf, que ocupa mucha memoria)
static void AgregarNumero ( char *texto , uint32_t &largo , int32_t numero )
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

/***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/

void Sheduller ( void )
{
	for ( uint8_t i = 0 ; i < PerifericoTemporizado::m_countPerifericosTemporizados  ; i ++ )
		g_perifericosTemporizados[i]->HandlerDelPeriferico();
}

void InicializarAplicacion ( void )
{
	t_Matriz.TimerStart( PERIODO_MATRIZ , EvMatriz , Timer::DEC );
	t_PC.TimerStart( PERIODO_PC , EvPC , Timer::DEC );
}

// Se llama todo el tiempo desde el while del main
void Aplicacion ( void )
{
	// Eventos de los timers (cuando vencen llaman a EvMatriz / EvPC)
	t_Matriz.TmrEvent();
	t_PC.TmrEvent();

	// Boton de la palanca: al APRETARLO el punto vuelve al centro
	uint8_t sw = g_swJoy1.get();
	if ( sw && !s_swAnterior )
	{
		s_fila = FILA_CENTRO;
		s_columna = COLUMNA_CENTRO;
	}
	s_swAnterior = sw;

	// LEDs: prendidos mientras el boton esta apretado
	g_out0 = g_izqJoy1.get();
	g_out1 = sw;
	g_out2 = g_derJoy1.get();
}

// Cada PERIODO_MATRIZ: mueve el punto y redibuja la matriz (con los botones apretados)
void EvMatriz ( void )
{
	t_Matriz.TimerStart( PERIODO_MATRIZ , EvMatriz , Timer::DEC );

	MoverPunto( g_joy1.GetDireccion() );

	g_matriz.Limpiar();
	g_matriz.SetPixel( s_fila , s_columna );

	if ( g_izqJoy1.get() )
		DibujarColumna( 0 );
	if ( g_derJoy1.get() )
		DibujarColumna( MATRIZ_COLUMNAS - 1 );
	if ( g_swJoy1.get() )
		DibujarFila( 0 );

	g_matriz.Actualizar();
}

// Pasa la posicion relativa al centro (-2048..+2047 aprox.) a 0..4095, sin salirse del rango
static int32_t AEscalaADC ( int32_t relativo )
{
	int32_t valor = Joystick::CENTRO_TEORICO + relativo;
	if ( valor < 0 )
		valor = 0;
	if ( valor > 4095 )
		valor = 4095;
	return valor;
}

// Agrega los datos de un joystick: x,y,sw,izq,der
// x e y se mandan YA corregidos (GetX/GetY respetan la inversion de ejes y el centro medido)
static void AgregarJoystick ( char *texto , uint32_t &largo , Joystick &joy ,
		DigitalInputs &sw , DigitalInputs &izq , DigitalInputs &der )
{
	AgregarNumero( texto , largo , AEscalaADC( joy.GetX() ) );	texto[ largo++ ] = ',';
	AgregarNumero( texto , largo , AEscalaADC( joy.GetY() ) );	texto[ largo++ ] = ',';
	AgregarNumero( texto , largo , sw.get() );									texto[ largo++ ] = ',';
	AgregarNumero( texto , largo , izq.get() );									texto[ largo++ ] = ',';
	AgregarNumero( texto , largo , der.get() );
}

// Cada PERIODO_PC: arma y envia  #JOY,x1,y1,sw1,izq1,der1,x2,y2,sw2,izq2,der2$
void EvPC ( void )
{
	t_PC.TimerStart( PERIODO_PC , EvPC , Timer::DEC );

	char linea[ 64 ];				// el mensaje mas largo tiene ~50 caracteres
	uint32_t largo = 0;
	const char *encabezado = "#JOY,";

	while ( *encabezado )
		linea[ largo++ ] = *encabezado++;

	AgregarJoystick( linea , largo , g_joy1 , g_swJoy1 , g_izqJoy1 , g_derJoy1 );
	linea[ largo++ ] = ',';
	AgregarJoystick( linea , largo , g_joy2 , g_swJoy2 , g_izqJoy2 , g_derJoy2 );
	linea[ largo++ ] = '$';
	linea[ largo++ ] = '\r';
	linea[ largo++ ] = '\n';

	uart0.Transmit( linea , largo );
}
