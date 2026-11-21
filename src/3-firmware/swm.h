/*******************************************************************************************************************************//**
 *
 * @file		swm.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		12 sep. 2023
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef CLASES_I0_USART_SWM_H_
#define CLASES_I0_USART_SWM_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include "tipos.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
// extern tipo nombreVariable;
/***********************************************************************************************************************************
 *** IMPLANTACION DE UNA CLASE
 **********************************************************************************************************************************/

#define		PIN_ASSIGN			(( volatile uint32_t*)	0x4000C000)

#define		PA_U0_TXD			0
#define		PA_U0_RXD			1
#define		PA_U0_RTS			2
#define		PA_U0_CTS			3
#define		PA_U0_SCLK			4
#define		PA_U1_TXD			5
#define		PA_U1_RXD			6
#define		PA_U1_RTS			7
#define		PA_U1_CTS			8
#define		PA_U1_SCLK			8
#define		PA_U2_TXD			10
#define		PA_U2_RXD			11
#define		PA_U2_RTS			12
#define		PA_U2_CTS			13
#define		PA_U2_SCLK			14
#define		PA_SPI0_SCK			15
#define		PA_SPI0_MOSI		16
#define		PA_SPI0_MISO		17
#define		PA_SPI0_SSEL0		18
#define		PA_SPI0_SSEL1		19
#define		PA_SPI0_SSEL2		20
#define		PA_SPI0_SSEL3		21
#define		PA_SPI1_SCK			22
#define		PA_SPI1_MOSI		23
#define		PA_SPI1_MISO		24
#define		PA_SPI1_SSEL0		25
#define		PA_SPI1_SSEL1		26
#define		PA_SCT_PIN0			27
#define		PA_SCT_PIN1			28
#define		PA_SCT_PIN2			29
#define		PA_SCT_PIN3			30
#define		PA_SCT_OUT0			31
#define		PA_SCT_OUT1			32
#define		PA_SCT_OUT2			33
#define		PA_SCT_OUT3			34
#define		PA_SCT_OUT4			35
#define		PA_SCT_OUT5			36
#define		PA_SCT_OUT6			37
#define		PA_I2C1_SDA			38
#define		PA_I2C1_SCL			39
#define		PA_I2C2_SDA			40
#define		PA_I2C2_SCL			41
#define		PA_I2C3_SDA			42
#define		PA_I2C3_SCL			43
#define		PA_ACMP_O			44
#define		PA_CLKOUT			45
#define		PA_GPIO_INT_BMAT	46
#define		PA_U3_TXD			47
#define		PA_U3_RXD			48
#define		PA_U3_SCLK			49
#define		PA_U4_TXD			50
#define		PA_U4_RXD			51
#define		PA_U4_SCLK			52
#define		PA_T0_MAT0			53
#define		PA_T0_MAT1			54
#define		PA_T0_MAT2			55
#define		PA_T0_MAT3			56
#define		PA_T0_CAP0			57
#define		PA_T0_CAP1			58
#define		PA_T0_CAP2			59

#define		PE_ACMP_I1			0
#define		PE_ACMP_I2			1
#define		PE_ACMP_I3			2
#define		PE_ACMP_I4			3
#define		PE_ACMP_I5			4
#define		PE_SWCLK			5
#define		PE_SWDIO			6
#define		PE_XTALIN			7
#define		PE_XTALOUT			8
#define		PE_RESETN			9
#define		PE_CLKIN			10
#define		PE_VDDCMP			11
#define		PE_I2C0_SDA			12
#define		PE_I2C0_SCL			13
#define		PE_ADC_0			14
#define		PE_ADC_1			15
#define		PE_ADC_2			16
#define		PE_ADC_3			17
#define		PE_ADC_4			18
#define		PE_ADC_5			19
#define		PE_ADC_6			20
#define		PE_ADC_7			21
#define		PE_ADC_8			22
#define		PE_ADC_9			23
#define		PE_ADC_10			24
#define		PE_ADC_11			25
#define		PE_DACOUT0			26
#define		PE_DACOUT1			27
#define		PE_CAPT_X0			28
#define		PE_CAPT_X1			29
#define		PE_CAPT_X2			30
#define		PE_CAPT_X3			31

#define		PE_CAPT_X4			32
#define		PE_CAPT_X5			33
#define		PE_CAPT_X6			34
#define		PE_CAPT_X7			35
#define		PE_CAPT_X8			35
#define		PE_CAPT_YL			36
#define		PE_CAPT_YH			37

#define		PINENABLE			( ( volatile uint32_t*)	0x4000C1C0)

void PINASSIGN_Config( uint8_t pin_movible , uint8_t port , uint8_t pin );
void PINENABLE_Config( uint8_t pin_config , uint8_t enable );

#endif /* CLASES_I0_USART_SWM_H_ */
