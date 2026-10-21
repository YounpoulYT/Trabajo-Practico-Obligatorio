/*******************************************************************************************************************************//**
 *
 * @file		clock.cpp
 * @brief		Reloj del sistema: FRO interno a 30 MHz
 *
 * Basado en Inic_OSC() de dr_pll.cpp (G. Soccodaro, 2023). Reemplaza a Inicializar_PLL(), que
 * dejaba el clock en un valor distinto del FREQ_CLOCK que usa el resto del codigo.
 *
 * Camino del reloj:   FRO (30 MHz) --> main_clk --> /1 --> clock del sistema (CPU y perifericos)
 *
 **********************************************************************************************************************************/

#include "clock.h"

// Funcion de la ROM del micro que ajusta el FRO. Recibe la frecuencia en kHz.
// La direccion es impar porque el Cortex-M0+ ejecuta en modo Thumb.
#define	FRO_SET_FREQUENCY_ROM_API	( 0x0F0026F5U )

#define	PDRUNCFG_FROOUT_PD			( 1 << 0 )		// salida del FRO (activo bajo: 0 = encendido)
#define	PDRUNCFG_FRO_PD				( 1 << 1 )		// FRO              (activo bajo: 0 = encendido)
#define	FROOSCCTRL_FRO_DIRECT		( 1 << 17 )		// 1 = usar el FRO sin dividir por 2

#define	MAINCLKSEL_FRO				0				// main_clk_pre_pll = FRO
#define	MAINCLKPLLSEL_SIN_PLL		0				// main_clk = main_clk_pre_pll (sin PLL)

void InicializarClock( void )
{
	// 1- Encender el FRO
	SYSCON->PDRUNCFG &= ~( PDRUNCFG_FROOUT_PD | PDRUNCFG_FRO_PD );

	// 2- Pedirle a la ROM que ajuste el FRO a 30 MHz (30000 kHz)
	(*( ( void (*)( uint32_t ) ) FRO_SET_FREQUENCY_ROM_API ))( FREQ_CLOCK / 1000 );

	// 3- Usar el FRO directo (sin el divisor por 2). Los registros xxxUEN hay que ponerlos
	//    en 0 y despues en 1 para que el cambio tenga efecto.
	SYSCON->FROOSCCTRL |= FROOSCCTRL_FRO_DIRECT;
	SYSCON->FRODIRECTCLKUEN = 0;
	SYSCON->FRODIRECTCLKUEN = 1;

	// 4- main_clk = FRO
	SYSCON->MAINCLKSEL = MAINCLKSEL_FRO;
	SYSCON->MAINCLKUEN = 0;
	SYSCON->MAINCLKUEN = 1;

	SYSCON->MAINCLKPLLSEL = MAINCLKPLLSEL_SIN_PLL;
	SYSCON->MAINCLKPLLUEN = 0;
	SYSCON->MAINCLKPLLUEN = 1;

	// 5- Clock del sistema = main_clk / 1
	SYSCON->SYSAHBCLKDIV = 1;
}
