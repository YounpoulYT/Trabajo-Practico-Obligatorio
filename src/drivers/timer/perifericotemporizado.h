/*******************************************************************************************************************************//**
 *
 * @file		perifericotemporizado.h
 * @brief		Clase base de todo lo que tiene que "atenderse" cada 1 ms (por ejemplo los Timer)
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 * Cada objeto que hereda de PerifericoTemporizado se anota solo en una lista al crearse.
 * El SysTick llama a AtenderTodos() cada 1 ms, y eso ejecuta HandlerDelPeriferico() de cada uno.
 * (Antes esto lo hacia la funcion Sheduller() de aplicacion.cpp.)
 *
 **********************************************************************************************************************************/

#ifndef PERIFERICOTEMPORIZADO_H_
#define PERIFERICOTEMPORIZADO_H_

#include "tipos.h"

class PerifericoTemporizado
{
	public:
		static const uint8_t MAX_PERIFERICOS = 20;

		PerifericoTemporizado() {}
		virtual ~PerifericoTemporizado() {}

		// Lo que hace el objeto cada 1 ms
		virtual void HandlerDelPeriferico( void ) = 0;

		// Llamar desde el SysTick: atiende a todos los objetos anotados
		static void AtenderTodos( void );

	protected:
		// Lo llaman las clases hijas en su constructor para anotarse en la lista
		static bool InstalarPerifericoTemporizado( PerifericoTemporizado *periferico );

	private:
		static PerifericoTemporizado	*s_lista[ MAX_PERIFERICOS ];
		static uint8_t					s_cantidad;
};

#endif /* PERIFERICOTEMPORIZADO_H_ */
