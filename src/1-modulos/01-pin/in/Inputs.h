/************************************************************
 * @file		inputs.h
 * @brief		clase abstracta pura de entradas
 * @date		18 oct. 2022
 * @author		Ing. Marcelo Trujillo
 *
 *************************************************************/
#ifndef INPUTS_H_
#define INPUTS_H_

/************************************************************
 *** INCLUDES GLOBALES
 ************************************************************/
#include "tipos.h"

class Inputs
{
	public:
		Inputs(){};
		virtual uint8_t SetDirInputs ( void )  = 0;
		virtual uint8_t GetPin ( void ) const = 0;
		virtual uint8_t SetPinModeIn ( void ) = 0;
		virtual ~Inputs(){};
};

#endif /* INPUTS_H_ */
