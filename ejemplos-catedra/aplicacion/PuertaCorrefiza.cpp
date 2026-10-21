/*
 * PuertaCorrefiza.cpp
 *
 *  Created on: 5 ago. 2026
 *      Author: Marcelo Trujillo
 */

#include <PuertaCorrefiza.h>
#include "inicializarInfotronic.h"

static void Reset ( void );
static void Cerrado ( void );
static void Abriendo ( void );
static void Abierto ( void );
static void Cerrando ( void );

enum estados {RESET,CERRADO,ABRIENDO,ABIERTO,CERRANDO};
enum accion {OFF,ON};

static uint8_t Estados = RESET ;
Timer Demora(Timer::SEG);

void (*PuertaCorediza[])(void) = {Reset,Cerrado,Abriendo,Abierto,Cerrando};

static void Reset ( void )
{
	MC = OFF;
	MA = ON;
	Estados = ABRIENDO;
}

static void Cerrado ( void )
{
	if ( SP )
	{
		MC = OFF;
		MA = ON;
		Estados = ABRIENDO;
	}
}
static void Abriendo ( void )
{
	if ( SA )
	{
		MA = OFF;
		MC = OFF;
		Demora = 2;
		Estados = ABIERTO;
	}
}
static void Abierto ( void )
{
	if ( SP )
		Demora = 2;

	if ( !SP && Demora )
	{
		MA = OFF;
		MC = ON ;
		Estados = CERRANDO;
	}
}
static void Cerrando ( void )
{
	if ( SP )
	{
		MC = OFF ;
		MA = ON;
		Estados = ABRIENDO;
		return;
	}

	if ( SC )
	{
		MC = OFF ;
		MA = OFF;
		Estados = CERRADO;
	}

}

void MaquinaDeEstados (void)
{
	if ( Estados > CERRANDO)
		Estados = RESET;
	PuertaCorediza[ Estados ]();
}
