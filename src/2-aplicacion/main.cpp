/*******************************************************************************************************************************//**
 *
 * @file		main.cpp
 * @brief		Programa principal del TPO
 * @date		12 nov. 2026
 * @author		Juan Pablo Klymenko Campos
 *
 * Los objetos de hardware (matriz, joystick, LEDs, uart) se crean en inicializarInfotronic.cpp.
 * Lo que hace el programa esta en aplicacion.cpp.
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "inicializarInfotronic.h"
#include "aplicacion.h"

int main ( void )
{
	// PLL, SysTick, matriz y calibracion del joystick (NO tocar la palanca al arrancar)
	InicializarInfotronic();
	InicializarAplicacion();

	while ( 1 )
	{
		Aplicacion();
	}
}
