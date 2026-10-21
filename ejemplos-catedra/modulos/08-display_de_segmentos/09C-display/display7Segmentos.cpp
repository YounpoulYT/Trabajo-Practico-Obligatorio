/*******************************************************************************************************************************//**
 *
 * @file		display.cpp
 * @brief		Descripcion del modulo
 * @date		26 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "display7Segmentos.h"


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
Display7Segmentos::Display7Segmentos(
		Segmentos * segmentos,
		Barrido *barrido,
		struct GrupoDeDigitos *GruposDeDigitos,
		uint8_t *pIndiceCorrector):
		m_segmentos(segmentos),
		m_barrido(barrido),
		m_GruposDeDigitos(GruposDeDigitos),
		m_pIndiceCorrector(pIndiceCorrector)
{
	m_ticks = 1;
	m_CantiadDeDigitos = 0 ;
	for(uint8_t j=0;m_GruposDeDigitos[ j ].Cantidad != -1 ; j++)
		m_CantiadDeDigitos += m_GruposDeDigitos[ j ].Cantidad ;
	
	m_aux = new uint8_t[m_CantiadDeDigitos];
	
	m_IndiceBarrido = 0;
	m_BufferDisplay = new uint8_t[ m_CantiadDeDigitos ];
	m_barrido->SetNDigitos( m_CantiadDeDigitos );
	InstalarPerifericoTemporizado(this);
}

void Display7Segmentos::HandlerDelPeriferico( )
{
	m_ticks--;
	if ( !m_ticks )
	{
		m_ticks = 1;
		m_segmentos->ClrSegmentos();
		m_barrido->SetDigito( );
		m_segmentos->SetSegmentos(m_BufferDisplay[m_IndiceBarrido]);
		m_IndiceBarrido ++;
		m_IndiceBarrido %= m_CantiadDeDigitos;
	}
}

void Display7Segmentos::SetDisplay( uint32_t valor , nDisplay dsp = DSP0 )
{
	uint8_t Comienzo = m_GruposDeDigitos[dsp].Comienzo ;
	uint8_t Cantidad = m_GruposDeDigitos[dsp].Cantidad ;
	uint8_t Fin = Comienzo + Cantidad ;

	for (uint8_t i = 0 ; i < Cantidad ; i ++,valor /= 10)
		m_aux[ i ] = valor % 10 ;
	
	for ( uint8_t i = 0; i < Cantidad ; i ++ )
	{
		if (!m_pIndiceCorrector)
			m_BufferDisplay[Fin-i-1] = m_aux [i];
		else
			m_BufferDisplay[m_pIndiceCorrector[Fin-i-1]] = m_aux[i];
	}
}

Display7Segmentos::~Display7Segmentos() {
	// TODO Auto-generated destructor stub
}
