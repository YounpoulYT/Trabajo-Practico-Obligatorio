/*
 * Timer.cpp
 *
 *  Created on: 22 may. 2024
 *      Author: Marcelo
 */

#include "Timer.h"

Timer::Timer()
{
	m_tmrRun = 0;
	m_tmrEvent = false;
	m_tmrStandBy = false;

	m_tmrHandler = nullptr ;
	m_tmrBase = DEC;
	InstalarPerifericoTemporizado(this);

}

Timer::Timer( const bases_t base , const Timer_Callback callback )
{
	m_tmrRun = 0;
	m_tmrEvent = false;
	m_tmrStandBy = false;

	m_tmrHandler = callback ;
	m_tmrBase = base;
	InstalarPerifericoTemporizado(this);

}
void Timer::HandlerDelPeriferico( void )
{
	if ( m_tmrStandBy == false )
	{
		if ( m_tmrRun )
		{
			m_tmrRun --;
			if ( !m_tmrRun )
				m_tmrEvent = true;
		}
	}
}

void Timer::TimerStart( uint32_t time, const Timer_Callback handler , const bases_t base )
{
	m_tmrBase = base;
	m_tmrHandler = handler;
	time = BaseToticks ( time ) ;
	m_tmrRun = time;

	if( m_tmrRun )
		m_tmrEvent = false ;
	else
		m_tmrEvent = true;
	return ;
}

bool Timer::TimerStart( uint32_t time )
{
	bool salida = false;
	if ( m_tmrHandler )
	{
		salida = true ;
		time = BaseToticks ( time ) ;
		m_tmrRun = time;

		if( m_tmrRun )
			m_tmrEvent = false ;
		else
			m_tmrEvent = true;
	}
	return salida ;
}

bool Timer::SetTimer( uint32_t time )
{
	bool salida = false;

	if ( m_tmrHandler )
	{
		if ( m_tmrRun )
		{
			salida = true ;
			time = BaseToticks ( time ) ;
			m_tmrRun = time;
			m_tmrEvent = false ;
		}
	}
	return salida ;
}

bool Timer::GetTimer( uint32_t &time )
{
	bool salida = false;

	if ( m_tmrHandler )
	{
		if ( m_tmrRun )
			time = TicksToBase( m_tmrRun );
	}
	return salida;
}

void Timer::StandByTimer( const standby_t accion )
{
	m_tmrStandBy = accion;

	return ;
}

void Timer::TimerStop( void )
{
	m_tmrRun = 0;
	m_tmrEvent = false ;
	m_tmrStandBy = 0 ;
	return ;
}

bool Timer::operator==( uint32_t t )
{
	if( (uint32_t) m_tmrRun == t )
	{
		return  true;
	}
	return false;
}

// Como esta definida una sobrecarga con explicit hace falta definir
// la sobrecarga doble en el orden de los parametros
// porque se perdio la promocion automatica de tipos

/**
	\fn bool operator==( uint32_t t , timer &T )
	\brief Sobrecarga de del operador de comparacion
	\details compara un valor numerico contra el flag de finalizacion del timer
	\return bool: true por coincidencia, false por no coincidencia
*/
//bool operator==( uint32_t t , timer &T  )
//{
//
//}

Timer& Timer::operator=( uint32_t time )
{
	m_tmrEvent = false ;
	m_tmrRun = BaseToticks ( time );
	return *this;
}


Timer::operator bool ()
{
	return m_tmrEvent ;
}

void Timer::TmrEvent ( void )
{
	if ( m_tmrEvent )
	{
		m_tmrEvent = false ;
		m_tmrHandler ();
	}
}


Timer::~Timer()
{

}
uint32_t Timer::BaseToticks( uint32_t time )
{
	switch ( m_tmrBase )
	{
		case DEC:
			time *= DECIMAS;
			break;
		case SEG:
			time *= DECIMAS * SEGUNDOS;
			break;
		case MIN:
			time *= DECIMAS * SEGUNDOS * MINUTOS ;
			break;
	}
	return time;
}

uint32_t Timer::TicksToBase( uint32_t time ) const
{
	switch ( m_tmrBase )
	{
		case DEC:
			time /= DECIMAS;
			break;
		case SEG:
			time /= ( SEGUNDOS * DECIMAS );
			break;
		case MIN:
			time /= ( MINUTOS * SEGUNDOS * DECIMAS );
			break;
	}
	return time;
}
