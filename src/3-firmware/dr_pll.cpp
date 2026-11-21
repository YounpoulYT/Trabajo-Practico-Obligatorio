/*******************************************************************************************************************************//**
 *
 * @file		dr_pll.c
 * @brief		Descripcion del modulo
 * @date		5 feb. 2020
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <3-firmware/dr_pll.h>
#include <LPC845.h>

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
 *** PROTOTIPO DE FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

 /***********************************************************************************************************************************
 *** FUNCIONES GLOBALES AL MODULO
 **********************************************************************************************************************************/
/**
	\fn  void PLL_Init( void )
	\brief Inicializa el PLL en 24 MHz,. Por default el FRO interno tiene 12Mhz
 	\author Ing. Gustavo Fresno
 	\date 5 feb. 2020
 	\param [in] void
 	\param [out] void
	\return void
*/
void Inicializar_PLL( void )
{
	SYSCON->SYSAHBCLKDIV             = 0x1;       //set clock divider for core to 1
	SYSCON->MAINCLKSEL               &= ~(0x03);  //set "main clock" to IRC oscillator, if not system will lock up when PLL turns off!(sec. 3.5.11)

	SYSCON->MAINCLKUEN               &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
	SYSCON->MAINCLKUEN               |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update

	SYSCON->SYSPLLCLKSEL             = 0;      	  //connect IRC oscillator to SYSTEM PLL (sec. 3.5.9)

	SYSCON->SYSPLLCLKUEN             &= ~(1);     //write a zero to SYSPLLUEN register (sec. 3.5.10), necessary for SYSPLLCLKSEL to update
	SYSCON->SYSPLLCLKUEN             |= 1;        //write a one to SYSPLLUEN register (sec. 3.5.10), necessary for SYSPLLCLKSEL to update

	SYSCON->PDRUNCFG                 |= (1<<7);   //power down the PLL before changing divider values (sec 3.5.35)
	SYSCON->SYSPLLCTRL               = 0x23;      // M=4 P=1

	//main_clk = fro * (M + 1) / P
	//main_clk = 24Mhz

	SYSCON->PDRUNCFG 				&= ~(1<<7);  //power up PLL after divider values changed (sec. 3.5.35)
	while((SYSCON->SYSPLLSTAT & 1) == 0);         //wait for PLL to lock

	SYSCON->MAINCLKPLLSEL            = 0x01;      //set system oscillator to the output of the PLL (sec. 3.5.11)
	SYSCON->MAINCLKPLLUEN            &= ~(1);     //write a zero to the MAINCLKUEN register (sec. 3.5.12), necessary for MAINCLKSEL to update
	SYSCON->MAINCLKPLLUEN            |= 1;        //write a one to the MAINCLKUEN register (sec. 3.5.12)
}

/**
	\fn  void Inic_OSC( void )
	\brief Inicializa el FRO en 30MHz y lo usa como system  clock
 	\author Gabriel Soccodaro
 	\date 25 Abr. 2023
 	\param [in] void
 	\param [out] void
	\return void
*/
void Inic_OSC(void){
	// 1-Asegura que FRO está encendido
	SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_FROOUT_PD_MASK;
	SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_FRO_PD_MASK;
	// 2-Se de frecuencia de FRO
	(*((void (*)(uint32_t freq))(CLOCK_FRO_SETTING_API_ROM_ADDRESS)))(30000);
	// 3-Elige FRO directo como salida de FRO la segunda y tercera instrucciones son necesarias para que el update del FRO tnega efecto
	SYSCON->FROOSCCTRL |= (1<<17);
	SYSCON->FRODIRECTCLKUEN &= ~(uint32_t)SYSCON_FRODIRECTCLKUEN_ENA_MASK;
	SYSCON->FRODIRECTCLKUEN |= (uint32_t)SYSCON_FRODIRECTCLKUEN_ENA_MASK;
	// 4-Se asegura que Main clk está encendido
	SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_SYSOSC_PD_MASK;
	// Elige System oscilator como clk externo
	SYSCON->EXTCLKSEL &= ~(uint32_t)SYSCON_EXTCLKSEL_SEL_MASK;
	// 5-Elige FRO como main clk
	SYSCON->MAINCLKSEL &= ~(uint32_t)SYSCON_MAINCLKPLLSEL_SEL_MASK;
	SYSCON->MAINCLKUEN &= ~(uint32_t)SYSCON_MAINCLKUEN_ENA_MASK;
	SYSCON->MAINCLKUEN |= (uint32_t)SYSCON_MAINCLKUEN_ENA_MASK;
	SYSCON->MAINCLKPLLSEL &= ~(uint32_t)SYSCON_MAINCLKPLLSEL_SEL_MASK;
	SYSCON->MAINCLKPLLUEN &= ~(uint32_t)SYSCON_MAINCLKPLLUEN_ENA_MASK;
	SYSCON->MAINCLKPLLUEN |= ~(uint32_t)SYSCON_MAINCLKPLLUEN_ENA_MASK;
	// 6-Setea el divisor de clock en 1
	SYSCON->SYSAHBCLKDIV = (uint32_t) 0x01;
}
