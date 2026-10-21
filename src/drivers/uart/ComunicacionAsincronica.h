/*******************************************************************************************************************************//**
 *
 * @file		ComunicacionAsincronica.h
 * @brief		Interfaz (clase abstracta pura) de una comunicacion serie asincronica
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 * El resto del programa habla con la PC a traves de esta interfaz, sin saber que hay una UART abajo.
 *
 **********************************************************************************************************************************/

#ifndef COMUNICACIONASINCRONICA_H_
#define COMUNICACIONASINCRONICA_H_

#include "tipos.h"

class ComunicacionAsincronica
{
	public:
		ComunicacionAsincronica() {}

		virtual void Transmitir( const char *texto ) = 0;							// texto terminado en '\0' (el '\0' no se envia)
		virtual void Transmitir( const uint8_t *datos , uint32_t cantidad ) = 0;
		virtual bool Recibir( uint8_t &dato ) = 0;								// false si no llego nada

		virtual ~ComunicacionAsincronica() {}
};

#endif /* COMUNICACIONASINCRONICA_H_ */
