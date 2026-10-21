/*******************************************************************************************************************************//**
 *
 * @file		clock.h
 * @brief		Reloj del sistema: FRO interno a 30 MHz
 *
 * Todo el proyecto calcula tiempos y velocidades con FREQ_CLOCK (definido en LPC845.h = 30 MHz):
 * SysTick, UART (baudios), SPI y ADC. Por eso el reloj se configura UNA vez, al principio,
 * y siempre con este valor.
 *
 **********************************************************************************************************************************/

#ifndef CLOCK_H_
#define CLOCK_H_

#include "LPC845.h"

// Deja el clock del sistema en FREQ_CLOCK (30 MHz) usando el FRO interno, sin PLL.
void InicializarClock( void );

#endif /* CLOCK_H_ */
