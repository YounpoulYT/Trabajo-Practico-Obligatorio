/*******************************************************************************************************************************//**
 *
 * @file		hardware.h
 * @brief		Todos los objetos de hardware del TPO, disponibles para cualquier archivo con "extern"
 *
 * Los objetos se CREAN una sola vez en hardware.cpp. Aca solo se declaran con "extern":
 * "existe un objeto con este nombre y este tipo, esta definido en otro archivo".
 * Cualquier .cpp que haga  #include "hardware.h"  puede usarlos (g_matriz, g_joy1, etc.).
 *
 * Orden de uso:  InicializarHardware()  y despues usar los objetos.
 *
 **********************************************************************************************************************************/

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include "pines.h"
#include "Gpio.h"
#include "Spi.h"
#include "Max7219.h"
#include "Adc.h"
#include "Joystick.h"
#include "Pulsador.h"
#include "Uart.h"

// ---------------------------------------------------------------- Matriz LED 8x32
extern Gpio		g_matrizLoad;
extern Spi		g_matrizSpi;
extern Max7219	g_matriz;

// ---------------------------------------------------------------- Joysticks
extern Adc		g_adcJoy1X;
extern Adc		g_adcJoy1Y;
extern Adc		g_adcJoy2X;
extern Adc		g_adcJoy2Y;
extern Joystick	g_joy1;
extern Joystick	g_joy2;

// ---------------------------------------------------------------- Botones de los joysticks
extern Pulsador	g_botonJoy1;
extern Pulsador	g_botonJoy2;

// ---------------------------------------------------------------- LEDs del kit
extern Gpio		g_led0;
extern Gpio		g_led1;
extern Gpio		g_led2;

// ---------------------------------------------------------------- PC
extern Uart		g_uartPC;

// ---------------------------------------------------------------- Funciones

// Clock, SysTick, matriz, ADC, UART y calibracion de joysticks. Devuelve false si algo fallo.
// No tocar los joysticks mientras se ejecuta (se mide el centro).
bool InicializarHardware( void );

// Milisegundos desde que arranco el programa
uint32_t Milisegundos( void );

// Espera activa (bloquea el programa). Usar solo al arrancar o en pruebas.
void Esperar( uint32_t ms );

#endif /* HARDWARE_H_ */
