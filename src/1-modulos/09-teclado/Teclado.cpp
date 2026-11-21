/*******************************************************************************************************************************//**
 *
 * @file		teclado.cpp
 * @brief		Descripcion del modulo
 * @date		24 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Teclado.h"

/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

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
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
int a;

Teclado::Teclado ( Gpio **r , Gpio **s  ):m_ret(r),m_scn(s)
{
	m_TeclaEstadoInicial = 0 ;
	m_TeclaCantidadDeRebotes  = NO_KEY;
	m_BufferTeclado = NO_KEY;
	m_MaxRebotes = 4 ;
	uint8_t i;
	if ( m_scn )
	{
		for (i = 0 ; m_scn[i] ; i++);
		m_MaxScans = i ;
	}

	for (i = 0 ; m_ret[i] ; i++);
	m_MaxReturns = i;

	InstalarPerifericoTemporizado(this);
}


Teclado::~Teclado() {
	// TODO Auto-generated destructor stub
}
///***********************************************************************************************************************************
// *** TABLAS PRIVADAS AL MODULO
// **********************************************************************************************************************************/
void Teclado::HandlerDelPeriferico ( void )
{
	uint8_t  tecla;
	tecla = TecladoHW ( );
	TecladoSW ( tecla );
}

uint8_t Teclado::TecladoHW ( void )
{
	uint8_t j;
	if ( m_scn )
	{
		for ( uint8_t i = 0 ; i < m_MaxScans ; i++)
		{
			// Pongo todos en estado neutro
			for ( uint8_t j = 0 ; j < m_MaxScans ; j++ )
				m_scn[j]->SetPin() ;

			// Activo el pin a chequear
			m_scn[i]->ClrPin();

			for ( j = 0 ; j < m_MaxReturns ; j++)
			{
				if ( m_ret[j]->GetPin( ) )
					return j + i * m_MaxReturns ;
			}

		}
	}
	else
	{
		for ( uint8_t j = 0 ; j < m_MaxReturns ; j++)
		{
			if ( m_ret[j]->GetPin( ) )
				return j  ;
		}
	}
	return NO_KEY;
}


void Teclado::TecladoSW ( uint8_t TeclaEstadoActual )
{
	if ( TeclaEstadoActual == NO_KEY )
	{
		// NoFue presionada o esta rebotando
		m_TeclaCantidadDeRebotes = 0;
		m_TeclaEstadoInicial = NO_KEY;
		return ;
	}

	if ( m_TeclaCantidadDeRebotes == 0 )
	{
		m_TeclaEstadoInicial = TeclaEstadoActual;
		m_TeclaCantidadDeRebotes ++;
		return;
	}

	if ( TeclaEstadoActual == m_TeclaEstadoInicial )
	{
		if ( m_TeclaCantidadDeRebotes < m_MaxRebotes )
		{
			m_TeclaCantidadDeRebotes ++;
			return;
		}

		if (m_TeclaCantidadDeRebotes == m_MaxRebotes )
		{
			m_BufferTeclado = TeclaEstadoActual;
			m_TeclaCantidadDeRebotes ++;
		}
	}
	else
	{
		m_TeclaCantidadDeRebotes = 0;
		m_TeclaEstadoInicial = NO_KEY;
	}
	return ;
}


uint8_t	Teclado::GetKey( void )
{
	uint8_t key = m_BufferTeclado;
	m_BufferTeclado = NO_KEY;
	return key;
}
