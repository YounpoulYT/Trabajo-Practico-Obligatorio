/*******************************************************************************************************************************//**
 *
 * @file		systick.cpp
 * @brief		SysTick: timer del nucleo Cortex-M0+ que genera una interrupcion periodica
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 **********************************************************************************************************************************/

#include "systick.h"

#define	MAX_TICKS		0xFFFFFF		// el contador del SysTick es de 24 bits

static void (* volatile s_callback)( void ) = nullptr;

void SysTick_Handler( void )
{
	if ( s_callback )
		s_callback();
}

void SysTick_InstalarCallBack( void (*callBack)( void ) )
{
	s_callback = callBack;
}

bool SysTick_Inicializar( uint32_t ms )
{
	// Cantidad de ciclos de clock que entran en "ms" milisegundos
	uint32_t ticks = ( FREQ_CLOCK / 1000 ) * ms;

	if ( ticks == 0 || ticks > MAX_TICKS )
		return false;

	SysTick->RELOAD = ticks - 1;	// cuenta desde ticks-1 hasta 0
	SysTick->CURR = 0;				// fuerza la recarga
	SysTick->CTRL = 7;				// ENABLE = 1, TICKINT = 1 (interrumpe), CLKSOURCE = 1 (clock del sistema)
	return true;
}
