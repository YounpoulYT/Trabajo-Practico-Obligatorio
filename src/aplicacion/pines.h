/*******************************************************************************************************************************//**
 *
 * @file		pines.h
 * @brief		MAPA DE PINES del TPO: que hay conectado en cada pin, cuales estan libres y cuales no tocar
 *
 * Fuente: esquematico del kit Infotronic (Kit/kit22025.pdf) + foto del conector (Kit/stick.jpg).
 * Si se cambia una conexion, se cambia ACA y en ningun otro lado.
 *
 * ================================================================================================================
 *  PIN     USO EN EL TPO                         EN EL KIT ESTA CONECTADO A               NOTAS
 * ================================================================================================================
 *  P0_16   MATRIZ CLK  (SPI0 SCK)                TX1 (conector HC-05 / ESP8266)           No enchufar BT/WiFi
 *  P0_17   MATRIZ LOAD (CS)                      RX1 (conector HC-05 / ESP8266)           No enchufar BT/WiFi
 *  P0_27   MATRIZ DIN  (SPI0 MOSI)               Scn0 del teclado (F1)                    No usar el teclado del kit
 *  P0_23   JOY 1 (HW-504) VRx -> VERTICAL  ADC3  BCDA del CD4511 - CN12-1                 Ver nota de los ejes abajo
 *  P0_22   JOY 1 (HW-504) VRy -> HORIZONTAL ADC4 BCDB del CD4511 - CN12-2
 *  P0_21   JOY 2 (MH)     VRx -> VERTICAL  ADC5  BCDC del CD4511 - CN12-3
 *  P0_20   JOY 2 (MH)     VRy -> HORIZONTAL ADC6 BCDD del CD4511 - CN12-4
 *  P0_26   SW JOYSTICK 1 (boton de la palanca)   Ret0 del teclado (C0)
 *  P0_15   SW JOYSTICK 2 (boton de la palanca)   Ret1 del teclado (C1)
 *  P0_25   UART0 TX -> PC                        USB de la placa LPC845-BRK (VCOM)        No sale al conector del kit
 *  P0_24   UART0 RX <- PC                        USB de la placa LPC845-BRK (VCOM)        No sale al conector del kit
 *  P1_0    LED 0 (indicador)                     Out0 -> ULN2003 -> LED rojo del kit
 *  P1_1    LED 1 (indicador)                     Out1 -> ULN2003 -> LED rojo del kit
 *  P1_2    LED 2 (indicador)                     Out2 -> ULN2003 -> LED rojo del kit
 * ----------------------------------------------------------------------------------------------------------------
 *  LIBRES (se pueden usar)
 * ----------------------------------------------------------------------------------------------------------------
 *  P0_28   -                                     Scn1 del teclado (F0)                    Propuesto: pulsador 1
 *  P0_8    -                                     Ret2 del teclado (C2)                    Propuesto: pulsador 2
 *  P0_6    - (ADC canal 1)                       Ain0: POT1 / NTC segun DIP S1, CN8-1     Entrada analogica extra
 *  P0_29   -                                     AOUT (CN7)
 *  P0_30   -                                     aux0 -> ULN2003 (salida de potencia)     Posible: solenoide del premio
 *  P0_31   -                                     aux1 (CN6)
 *  P0_12   -                                     DP del display (transistor)
 *  P0_18   -                                     RST del CD4017 (display)                 Solo digital
 *  P0_19   -                                     CK del CD4017 (display)                  Solo digital
 *  P0_4    -                                     I0: microswitch uSW1 a GND, CN10 / CN9
 *  P0_0    -                                     LCD EN  (pull-up 10k a 5 V)              Solo digital
 *  P0_1    -                                     LCD RS  (pull-up 10k a 5 V)              Solo digital
 *  P0_9    -                                     LCD D4  (pull-up 10k a 5 V)              Solo digital
 *  P0_10   -                                     LCD D5  (pull-up 10k a 5 V)              Solo digital
 *  P0_11   -                                     LCD D6  (pull-up 10k a 5 V)              Solo digital
 *  P0_13   -                                     LCD D7  (pull-up 10k a 5 V)              Solo digital, NO como ADC
 *  P0_14   -                                     LCD E   (pull-up 10k a 5 V)              Solo digital, NO como ADC
 *  P0_7    - (ADC canal 0)                       No sale al conector del kit
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

// ---------------------------------------------------------------- Matriz LED 8x32 (4 x MAX7219 por SPI0)
#define	MATRIZ_SPI				0				// SPI0
#define	MATRIZ_CLK_PUERTO		Gpio::PORT0
#define	MATRIZ_CLK_PIN			16
#define	MATRIZ_DIN_PUERTO		Gpio::PORT0
#define	MATRIZ_DIN_PIN			27
#define	MATRIZ_LOAD_PUERTO		Gpio::PORT0
#define	MATRIZ_LOAD_PIN			17
#define	MATRIZ_VELOCIDAD_SPI	500000UL		// 500 kHz (el MAX7219 admite hasta 10 MHz)

// ---------------------------------------------------------------- Joysticks (canales del ADC)
// El pin "+5V" de los modulos va a 3,3 V (CN8-2 o CN10-5). El ADC no admite mas de 3,3 V.
//
// NOTA DE LOS EJES: con los modulos en la posicion en que se usan, la salida VRx del modulo
// cambia al mover la palanca ARRIBA/ABAJO y VRy al mover IZQUIERDA/DERECHA (se comprobo con el
// display de 7 segmentos, que lee estas mismas lineas: arriba J1 = 1, derecha J1 = 2,
// arriba J2 = 4, derecha J2 = 8). Por eso el eje X del JUEGO (horizontal) sale de VRy y el Y de VRx.
// Arriba y derecha dan tension ALTA -> no hace falta invertir.
#define	JOY1_X_CANAL			4				// P0_22 - VRy del HW-504 (horizontal)
#define	JOY1_Y_CANAL			3				// P0_23 - VRx del HW-504 (vertical)
#define	JOY2_X_CANAL			6				// P0_20 - VRy del MH (horizontal)
#define	JOY2_Y_CANAL			5				// P0_21 - VRx del MH (vertical)

// ---------------------------------------------------------------- Botones de los joysticks (pin SW del modulo, a GND al apretar)
// El modulo no trae resistencia: se usa el pull-up interno del LPC845.
#define	JOY1_SW_PUERTO			Gpio::PORT0
#define	JOY1_SW_PIN				26				// C0
#define	JOY2_SW_PUERTO			Gpio::PORT0
#define	JOY2_SW_PIN				15				// C1

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
