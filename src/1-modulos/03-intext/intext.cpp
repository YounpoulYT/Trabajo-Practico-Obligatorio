/*******************************************************************************************************************************//**
 *
 * @file		intext.cpp
 * @brief		Descripcion del modulo
 * @date		13 jun. 2023
 * @author		Ing. Federico Bua
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "intext.h"

#include "LPC845.h"
#include "tipos.h"
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
static Intext* intexts[8];
/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
#define SYSAHBCLKCTRL0_GPIO_INT_BIT 28
#define NVIC_PININT0_BIT 24

Intext::Intext(uint8_t pint, uint8_t port, uint8_t pin, void (*callback) (void))
{
	m_pint = pint;
	m_port = port;
	m_pin = pin;
	m_callback = callback;
}

void Intext::setup()
{
	SYSCON->PINTSEL[m_pint] = m_port*32 + m_pin;
	SYSCON->SYSAHBCLKCTRL0 |= (1 << SYSAHBCLKCTRL0_GPIO_INT_BIT);
	SYSCON->STARTERP0 &= ~(1 << m_pint);
	NVIC->ISER[0] |= (1 << (NVIC_PININT0_BIT+m_pint));
	intexts[m_pint] = this;
}

void Intext::set_callback(void (*callback) (void))
{
	m_callback = callback;
}

int Intext::set_flanco(Intext::flanco_t flanco)
{
	if((m_pint > 7 ) || (m_port > 1) || (m_pin > 31))
		return -1;
	setup();
	// seleccion x flanco -> ponemos un cero en la 
	// posicion el bit de interrupcion

	PINT->ISEL &= ~(1 << m_pint);		
	PINT->IENF &= ~(1 << m_pint);
	PINT->IENR &= ~(1 << m_pint);

	if((flanco == ASCENDENTE) || (flanco == AMBOS_FLANCOS))
		PINT->IENR |= (1 << m_pint);
	if((flanco == DESCENDENTE) || (flanco == AMBOS_FLANCOS))
		PINT->IENF |= (1 << m_pint);

	return 0;
}

int Intext::set_nivel(Intext::nivel_t nivel)
{
	if((m_pint > 7 ) || (m_port > 1) || (m_pin > 31))
		return -1;
	setup();
	PINT->ISEL |= (1 << m_pint);
	PINT->IENF &= ~(1 << m_pint);
	PINT->IENF |= (nivel << m_pint);
	PINT->IENR |= (1 << m_pint);
	return 0;
}

void Intext::limpiar_irq()
{
	if(PINT->IST & (1 << m_pint))
	{
		PINT->IST |= (1 << m_pint); //En caso de ser nivel, cambia el nivel para que no interrumpa siempre
		if(m_callback)
			m_callback();
	}
}

void PININT0_IRQHandler(void)
{
	intexts[0]->limpiar_irq();
}


void PININT1_IRQHandler(void)
{
	intexts[1]->limpiar_irq();
}
void PININT2_IRQHandler(void){ intexts[2]->limpiar_irq();}
void PININT3_IRQHandler(void){ intexts[3]->limpiar_irq();}
void PININT4_IRQHandler(void){ intexts[4]->limpiar_irq();}
void PININT5_IRQHandler(void){ intexts[5]->limpiar_irq();}
//void PININT6_IRQHandler(void){ intexts[6]->limpiar_irq();}
//void PININT7_IRQHandler(void){ intexts[7]->limpiar_irq();}
