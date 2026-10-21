/*******************************************************************************************************************************//**
 *
 * @file		systick.h
 * @brief		SysTick: timer del nucleo Cortex-M0+ que genera una interrupcion periodica
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 **********************************************************************************************************************************/

#ifndef SYSTICK_H_
#define SYSTICK_H_

#include "LPC845.h"

// Funcion que se va a llamar en cada interrupcion del SysTick
void SysTick_InstalarCallBack( void (*callBack)( void ) );

// Arranca el SysTick con un periodo de "ms" milisegundos. Devuelve false si el valor no entra.
bool SysTick_Inicializar( uint32_t ms );

#if defined (__cplusplus)
extern "C" {
	void SysTick_Handler( void );
}
#endif

#endif /* SYSTICK_H_ */
