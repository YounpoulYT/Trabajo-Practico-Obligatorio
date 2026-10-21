/*******************************************************************************************************************************//**
 *
 * @file		perifericotemporizado.cpp
 * @brief		Lista de objetos que se atienden cada 1 ms
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 **********************************************************************************************************************************/

#include "perifericotemporizado.h"

PerifericoTemporizado	*PerifericoTemporizado::s_lista[ MAX_PERIFERICOS ];
uint8_t					PerifericoTemporizado::s_cantidad = 0;

bool PerifericoTemporizado::InstalarPerifericoTemporizado( PerifericoTemporizado *periferico )
{
	if ( s_cantidad >= MAX_PERIFERICOS )
		return false;			// lista llena: agrandar MAX_PERIFERICOS

	s_lista[ s_cantidad ] = periferico;
	s_cantidad++;
	return true;
}

void PerifericoTemporizado::AtenderTodos( void )
{
	for ( uint8_t i = 0 ; i < s_cantidad ; i++ )
		s_lista[ i ]->HandlerDelPeriferico();
}
