/*******************************************************************************************************************************//**
 *
 * @file		Outputs.h
 * @brief		Interfaz (clase abstracta pura) de una salida digital
 * @author		Ing. Marcelo Trujillo
 *
 * Max7219 recibe un Outputs* para el pin LOAD: no le importa si es un Gpio u otra cosa,
 * solo que se pueda activar y desactivar.
 *
 **********************************************************************************************************************************/

#ifndef OUTPUTS_H_
#define OUTPUTS_H_

#include "tipos.h"

class Outputs
{
	public:
		Outputs() {}
		virtual uint8_t SetPin( void ) = 0;
		virtual uint8_t ClrPin( void ) = 0;
		virtual uint8_t SetTogglePin( void ) = 0;
		virtual uint8_t SetDirOutputs( void ) = 0;
		virtual uint8_t SetPinModeOut( void ) = 0;
		virtual ~Outputs() {}
};

#endif /* OUTPUTS_H_ */
