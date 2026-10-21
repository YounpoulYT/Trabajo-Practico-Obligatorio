/*******************************************************************************************************************************//**
 *
 * @file		Intext.cpp
 * @brief		Interrupcion externa por pin (PININT)
 * @author		Ing. Federico Bua (simplificado para el TPO)
 *
 * Cambios: se borra el flag con "=" (con "|=" se borraban TODAS las interrupciones pendientes),
 * se chequea que exista el objeto antes de llamarlo, y se habilitan PININT6 y PININT7
 * (antes estaban tomadas por UART3 / UART4, que ya no se usan).
 *
 **********************************************************************************************************************************/

#include "Intext.h"

#define	CLOCK_GPIO_INT		28		// bit de SYSAHBCLKCTRL0
#define	NVIC_PININT0		24		// PININT0 es la interrupcion 24; PININT7 es la 31

static Intext *s_intexts[ 8 ] = { nullptr };

Intext::Intext( uint8_t pint , uint8_t port , uint8_t pin , void (*callback)( void ) )
{
	m_pint = pint;
	m_port = port;
	m_pin = pin;
	m_callback = callback;
}

void Intext::setup( void )
{
	SYSCON->PINTSEL[ m_pint ] = m_port * 32 + m_pin;			// que pin vigila esta interrupcion
	SYSCON->SYSAHBCLKCTRL0 |= ( 1 << CLOCK_GPIO_INT );
	s_intexts[ m_pint ] = this;
	NVIC->ISER[0] = ( 1 << ( NVIC_PININT0 + m_pint ) );
}

void Intext::set_callback( void (*callback)( void ) )
{
	m_callback = callback;
}

int Intext::set_flanco( flanco_t flanco )
{
	if ( m_pint > 7 || m_port > 1 || m_pin > 31 )
		return -1;

	setup();

	PINT->ISEL &= ~( 1 << m_pint );		// 0 = por flanco
	PINT->IENR &= ~( 1 << m_pint );
	PINT->IENF &= ~( 1 << m_pint );

	if ( flanco == ASCENDENTE || flanco == AMBOS_FLANCOS )
		PINT->IENR |= ( 1 << m_pint );	// flanco ascendente (0 V -> 3,3 V)
	if ( flanco == DESCENDENTE || flanco == AMBOS_FLANCOS )
		PINT->IENF |= ( 1 << m_pint );	// flanco descendente (3,3 V -> 0 V)

	return 0;
}

int Intext::set_nivel( nivel_t nivel )
{
	if ( m_pint > 7 || m_port > 1 || m_pin > 31 )
		return -1;

	setup();

	PINT->ISEL |= ( 1 << m_pint );		// 1 = por nivel
	PINT->IENR |= ( 1 << m_pint );		// habilita
	if ( nivel == ALTO )
		PINT->IENF |= ( 1 << m_pint );	// en modo nivel, IENF elige el nivel activo
	else
		PINT->IENF &= ~( 1 << m_pint );

	return 0;
}

void Intext::Atender( void )
{
	if ( PINT->IST & ( 1 << m_pint ) )
	{
		PINT->IST = ( 1 << m_pint );	// borrar SOLO este flag (escribir 1 borra)
		if ( m_callback )
			m_callback();
	}
}

static void AtenderPinint( uint8_t n )
{
	if ( s_intexts[ n ] )
		s_intexts[ n ]->Atender();
}

void PININT0_IRQHandler( void ) { AtenderPinint( 0 ); }
void PININT1_IRQHandler( void ) { AtenderPinint( 1 ); }
void PININT2_IRQHandler( void ) { AtenderPinint( 2 ); }
void PININT3_IRQHandler( void ) { AtenderPinint( 3 ); }
void PININT4_IRQHandler( void ) { AtenderPinint( 4 ); }
void PININT5_IRQHandler( void ) { AtenderPinint( 5 ); }
void PININT6_IRQHandler( void ) { AtenderPinint( 6 ); }
void PININT7_IRQHandler( void ) { AtenderPinint( 7 ); }
