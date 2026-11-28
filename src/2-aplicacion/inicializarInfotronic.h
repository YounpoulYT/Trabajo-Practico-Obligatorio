/*******************************************************************************************************************************//**
 *
 * @file		inicializar.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		5 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef INICIALIZAR_H_
#define INICIALIZAR_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/

#include "Timer.h"
#include "digital_inputs.h"
#include "digital_outputs.h"
#include "Adc.h"
#include "Uart.h"
#include "dr_pll.h"
#include "Gpio.h"
#include "systick.h"
#include "SPI.h"
#include "Joystick.h"
#include "MAX7219.h"
#include "QTcreator.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define		SP		g_in0
#define		SA		g_in1
#define		SC		g_in2

#define		MA		g_out0
#define		MC		g_out1

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
// Entradas digitales
extern Gpio g_in0;
extern Gpio g_in1;
extern Gpio g_in2;

// Salidas digitales
extern Gpio g_out0 ;
extern Gpio g_out1 ;
extern Gpio g_out2 ;

// ADC
extern Adc g_canal1;

// Matriz LED 16x32 (8 x MAX7219 por SPI0)
extern Gpio g_loadMatriz;
extern Spi g_spiMatriz;
extern Max7219 g_matriz;

// Joystick 1: palanca (un Adc por eje) y sus tres botones
extern Adc g_adcJoy1X;
extern Adc g_adcJoy1Y;
extern Joystick g_joy1;
extern DigitalInputs g_swJoy1;		// boton de la palanca
extern DigitalInputs g_derJoy1;		// pulsador derecho
extern DigitalInputs g_izqJoy1;		// pulsador izquierdo

// Joystick 2: igual que el 1
extern Adc g_adcJoy2X;
extern Adc g_adcJoy2Y;
extern Joystick g_joy2;
extern DigitalInputs g_swJoy2;		// boton de la palanca
extern DigitalInputs g_derJoy2;		// pulsador derecho
extern DigitalInputs g_izqJoy2;		// pulsador izquierdo

// Uart
extern Uart uart0;

//QTcretor
extern QTcreator g_pc;

/***********************************************************************************************************************************
 *** PROTOTIPOS GLOBALES
 **********************************************************************************************************************************/
void InicializarInfotronic ( void ) ;
int32_t TablaNTC( int32_t dato);
#endif /* INICIALIZAR_H_ */
