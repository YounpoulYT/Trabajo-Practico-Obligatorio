/*******************************************************************************************************************************//**
 *
 * @file		pines.h
 * @brief		MAPA DE PINES del TPO: que hay conectado en cada pin, cuales estan libres y cuales no tocar
 *
 * Fuente: esquematico del kit Infotronic (Kit/kit22025.pdf) + foto del conector (Kit/stick.jpg).
 * Si se cambia una conexion, se cambia ACA y en ningun otro lado.
 *
 * ================================================================================================================
 *  PIN     USO EN EL TPO                         CABLE      EN EL KIT ESTA CONECTADO A               NOTAS
 * ================================================================================================================
 *  P0_16   MATRIZ CLK  (SPI0 SCK)                naranja    TX1 (conector HC-05 / ESP8266)           No enchufar BT/WiFi
 *  P0_17   MATRIZ CS / LOAD                      amarillo   RX1 (conector HC-05 / ESP8266)           No enchufar BT/WiFi
 *  P0_18   MATRIZ DIN  (SPI0 MOSI)               verde      RST del CD4017 (display)
 *  P0_19   JOY 1 SW (boton de la palanca)        blanco     CK del CD4017 (display)
 *  P0_20   JOY 1 VRy -> HORIZONTAL  ADC6         azul       BCDD del CD4511 - CN12-4                 Ver nota de los ejes abajo
 *  P0_21   JOY 1 VRx -> VERTICAL    ADC5         gris       BCDC del CD4511 - CN12-3
 *  P0_22   JOY 1 pulsador DERECHO                naranja    BCDB del CD4511 - CN12-2
 *  P0_23   JOY 1 pulsador IZQUIERDO              verde      BCDA del CD4511 - CN12-1
 *  P0_25   UART0 TX -> PC                                   USB de la placa LPC845-BRK (VCOM)        No sale al conector del kit
 *  P0_24   UART0 RX <- PC                                   USB de la placa LPC845-BRK (VCOM)        No sale al conector del kit
 *  P1_0    LED 0 (indicador)                                Out0 -> ULN2003 -> LED rojo del kit
 *  P1_1    LED 1 (indicador)                                Out1 -> ULN2003 -> LED rojo del kit
 *  P1_2    LED 2 (indicador)                                Out2 -> ULN2003 -> LED rojo del kit
 * ----------------------------------------------------------------------------------------------------------------
 *  RESERVADOS PARA EL JOYSTICK 2 (todavia NO conectado, mismo orden y colores que el 1)
 * ----------------------------------------------------------------------------------------------------------------
 *  P0_26   JOY 2 SW (boton de la palanca)        blanco     Ret0 del teclado (C0)
 *  P0_27   JOY 2 VRy                             azul       Scn0 del teclado (F1)                    OJO: este pin NO tiene ADC
 *  P0_28   JOY 2 VRx                             gris       Scn1 del teclado (F0)                    OJO: este pin NO tiene ADC
 *  P0_29   JOY 2 pulsador DERECHO                naranja    AOUT (CN7)
 *  P0_30   JOY 2 pulsador IZQUIERDO              verde      aux0 -> ULN2003 (salida de potencia)     Revisar: el ULN2003 carga el pin
 *
 *  OJO con P0_27 y P0_28: el LPC845 solo puede leer tensiones analogicas en los pines de la lista
 *  de canales de abajo. Los ejes del joystick 2 no se pueden leer en 27 y 28: hay que resolverlo
 *  antes de conectarlo (ver CONEXIONES.txt).
 * ----------------------------------------------------------------------------------------------------------------
 *  FALTA DEFINIR
 * ----------------------------------------------------------------------------------------------------------------
 *  ?       SERVO (motor, senal PWM)                                                                  Todavia sin pin
 * ----------------------------------------------------------------------------------------------------------------
 *  LIBRES (se pueden usar)
 * ----------------------------------------------------------------------------------------------------------------
 *  P0_15   -                                                Ret1 del teclado (C1)
 *  P0_8    -                                                Ret2 del teclado (C2)
 *  P0_6    - (ADC canal 1)                                  Ain0: POT1 / NTC segun DIP S1, CN8-1     Entrada analogica extra
 *  P0_31   -                                                aux1 (CN6)
 *  P0_12   -                                                DP del display (transistor)
 *  P0_4    - (ADC canal 11)                                 I0: microswitch uSW1 a GND, CN10 / CN9
 *  P0_0    -                                                LCD EN  (pull-up 10k a 5 V)              Solo digital
 *  P0_1    -                                                LCD RS  (pull-up 10k a 5 V)              Solo digital
 *  P0_9    -                                                LCD D4  (pull-up 10k a 5 V)              Solo digital
 *  P0_10   -                                                LCD D5  (pull-up 10k a 5 V)              Solo digital
 *  P0_11   -                                                LCD D6  (pull-up 10k a 5 V)              Solo digital
 *  P0_13   -                                                LCD D7  (pull-up 10k a 5 V)              Solo digital, NO como ADC
 *  P0_14   -                                                LCD E   (pull-up 10k a 5 V)              Solo digital, NO como ADC
 *  P0_7    - (ADC canal 0)                                  No sale al conector del kit
 * ----------------------------------------------------------------------------------------------------------------
 *  NO TOCAR
 * ----------------------------------------------------------------------------------------------------------------
 *  P0_2    SWDIO  (depurador)
 *  P0_3    SWCLK  (depurador) - tambien figura como I1 en el kit
 *  P0_5    RESET
 * ================================================================================================================
 *
 * Los canales del ADC tienen pin FIJO (no se pueden mover con la switch matrix):
 *   canal: 0=P0_7  1=P0_6  2=P0_14  3=P0_23  4=P0_22  5=P0_21  6=P0_20  7=P0_19  8=P0_18  9=P0_17  10=P0_13  11=P0_4
 * UART, SPI y PWM (SCT) si se pueden poner en cualquier pin digital.
 *
 **********************************************************************************************************************************/

#ifndef PINES_H_
#define PINES_H_

#include "Gpio.h"

// ---------------------------------------------------------------- Matriz LED 16x32 (8 x MAX7219 por SPI0)
// Son dos tiras de 8x32. La primera (su DIN va al micro) es la de ARRIBA.
// El DOUT de la tira de arriba va al DIN de la tira de abajo. CLK y CS van a las dos.
#define	MATRIZ_SPI				0				// SPI0
#define	MATRIZ_CLK_PUERTO		Gpio::PORT0
#define	MATRIZ_CLK_PIN			16				// cable naranja
#define	MATRIZ_LOAD_PUERTO		Gpio::PORT0
#define	MATRIZ_LOAD_PIN			17				// cable amarillo (CS)
#define	MATRIZ_DIN_PUERTO		Gpio::PORT0
#define	MATRIZ_DIN_PIN			18				// cable verde
#define	MATRIZ_VELOCIDAD_SPI	500000UL		// 500 kHz (el MAX7219 admite hasta 10 MHz)

// ---------------------------------------------------------------- Joystick 1: palanca (canales del ADC)
// El pin "+5V" del modulo va a 3,3 V (CN8-2 o CN10-5). El ADC no admite mas de 3,3 V.
//
// NOTA DE LOS EJES: con el modulo en la posicion en que se usa, la salida VRx cambia al mover
// la palanca ARRIBA/ABAJO y VRy al mover IZQUIERDA/DERECHA. Por eso el eje X del JUEGO
// (horizontal) sale de VRy y el Y de VRx. Si un eje queda al reves se invierte en hardware.cpp.
#define	JOY1_X_CANAL			6				// P0_20 - VRy (horizontal) - cable azul
#define	JOY1_Y_CANAL			5				// P0_21 - VRx (vertical)   - cable gris

// ---------------------------------------------------------------- Joystick 1: botones (a GND al apretar)
// No llevan resistencia: se usa el pull-up interno del LPC845.
#define	JOY1_SW_PUERTO			Gpio::PORT0
#define	JOY1_SW_PIN				19				// boton de la palanca - cable blanco
#define	JOY1_DER_PUERTO			Gpio::PORT0
#define	JOY1_DER_PIN			22				// pulsador derecho   - cable naranja
#define	JOY1_IZQ_PUERTO			Gpio::PORT0
#define	JOY1_IZQ_PIN			23				// pulsador izquierdo - cable verde

// ---------------------------------------------------------------- Joystick 2: TODAVIA NO CONECTADO
// Queda anotado para cuando se conecte. Mismo orden y colores que el joystick 1.
// Los ejes (VRy en 27, VRx en 28) no tienen define de canal porque esos pines no tienen ADC.
#define	JOY2_SW_PUERTO			Gpio::PORT0
#define	JOY2_SW_PIN				26				// boton de la palanca - cable blanco
#define	JOY2_DER_PUERTO			Gpio::PORT0
#define	JOY2_DER_PIN			29				// pulsador derecho   - cable naranja
#define	JOY2_IZQ_PUERTO			Gpio::PORT0
#define	JOY2_IZQ_PIN			30				// pulsador izquierdo - cable verde

// ---------------------------------------------------------------- LEDs del kit (P1_0..P1_2 -> ULN2003 -> LED)
// Pin en 3,3 V -> el ULN2003 conduce -> el LED prende. Por eso son activos en ALTO.
#define	LED0_PUERTO				Gpio::PORT1
#define	LED0_PIN				0
#define	LED1_PUERTO				Gpio::PORT1
#define	LED1_PIN				1
#define	LED2_PUERTO				Gpio::PORT1
#define	LED2_PIN				2

// ---------------------------------------------------------------- Comunicacion con la PC (Qt)
#define	UART_PC					0				// UART0
#define	UART_PC_TX_PUERTO		Gpio::PORT0
#define	UART_PC_TX_PIN			25
#define	UART_PC_RX_PUERTO		Gpio::PORT0
#define	UART_PC_RX_PIN			24
#define	UART_PC_BAUDIOS			9600

#endif /* PINES_H_ */
