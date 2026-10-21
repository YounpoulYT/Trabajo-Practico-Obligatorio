/*
 * Puerta.cpp
 *
 *  Created on: 6 ago. 2026
 *      Author: Marcelo Trujillo
 */

#include <Puerta.h>
#include "inicializarInfotronic.h"
#include "Timer.h"

enum estados {RESET_,CERRADO_,ABRIENDO_,ABIERTO_,CERRANDO_};
enum accion {Off,On};

static uint8_t Reset ( uint8_t );
static uint8_t Cerrado ( uint8_t );
static uint8_t Abriendo ( uint8_t );
static uint8_t Abierto ( uint8_t );
static uint8_t Cerrando ( uint8_t );

uint8_t (*VectorPuerta[])(uint8_t) = {Reset,Cerrado,Abriendo,Abierto,Cerrando};
Timer demora(Timer::SEG);

static uint8_t Reset ( uint8_t estado )
{
	MC = Off;
	MA = On;
	return ABRIENDO_;

}
static uint8_t Cerrado ( uint8_t estado )
{
	if ( SP )
	{
		MC = Off;
		MA = On;
		estado = ABRIENDO_;
	}
	return estado;
}

static uint8_t Abriendo ( uint8_t  estado )
{
	if (SA)
	{
		MA = Off;
		MC = Off;
		demora = 4;
		estado = ABIERTO_;
	}
	return estado ;
}

static uint8_t Abierto ( uint8_t  estado )
{
	if ( SP )
	{
		demora = 4;
	}

	if (!SP && demora )
	{
		MA = Off ;
		MC = On ;
		estado = CERRANDO_;
	}

	return estado;
}

static uint8_t Cerrando ( uint8_t  estado )
{
	if ( SC )
	{
		MC = Off;
		MA = Off;
		estado = CERRADO_;
	}

	if ( SP )
	{
		MC = Off;
		MA = On;
		estado = ABRIENDO_;
	}
	return estado;
}

void MaquinaDeEstadosPuerta ( void )
{
	static uint8_t Estado = RESET_;
	if ( Estado > CERRANDO_)
		Estado = RESET_;
	Estado = VectorPuerta[Estado]( Estado);
}


