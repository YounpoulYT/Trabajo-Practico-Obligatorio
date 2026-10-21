/*******************************************************************************************************************************//**
 *
 * @file		Gpio.cpp
 * @brief		Pin digital de entrada o salida
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 **********************************************************************************************************************************/

#include "Gpio.h"

#define	CLOCK_GPIO0			6		// bits de SYSAHBCLKCTRL0
#define	CLOCK_IOCON			18
#define	CLOCK_GPIO1			20

#define	IOCON_MODE_SHIFT	3		// bits 4:3 = pull-up / pull-down
#define	IOCON_MODE_MASK		( 0x3 << IOCON_MODE_SHIFT )
#define	IOCON_OD			( 1 << 10 )		// open drain (colector abierto)

// Indice de cada pin dentro de IOCON->PIO[] (manual LPC845, tabla de registros IOCON).
// Corregidos respecto de la version de la catedra: P0_8 (era 4), P0_29, P0_30 y P1_2.
static const uint8_t IOCON_INDEX_PIO0[32] =
{
	17 , 11 ,  6 ,  5 ,  4 ,  3 , 16 , 15 , 14 , 13 ,	// P0_0  .. P0_9
	 8 ,  7 ,  2 ,  1 , 18 , 10 ,  9 ,  0 , 30 , 29 ,	// P0_10 .. P0_19
	28 , 27 , 26 , 25 , 24 , 23 , 22 , 21 , 20 , 50 ,	// P0_20 .. P0_29
	51 , 35												// P0_30 .. P0_31
};

static const uint8_t IOCON_INDEX_PIO1[22] =
{
	36 , 37 , 38 , 41 , 42 , 43 , 46 , 49 , 31 , 32 ,	// P1_0  .. P1_9
	55 , 54 , 33 , 34 , 39 , 40 , 44 , 45 , 47 , 48 ,	// P1_10 .. P1_19
	52 , 53												// P1_20 .. P1_21
};

uint8_t Gpio::IndiceIOCON( uint8_t port , uint8_t bit )
{
	if ( port == PORT0 )
		return IOCON_INDEX_PIO0[ bit ];
	return IOCON_INDEX_PIO1[ bit ];
}

Gpio::Gpio( uint8_t port , uint8_t bit , uint8_t mode , uint8_t direction , uint8_t activity ) :
	m_port( port ) , m_bit( bit ) , m_modo( mode ) , m_direccion( direction ) , m_actividad( activity )
{
	m_error = OK;

	if ( m_port > PORT1 || ( m_port == PORT0 && m_bit > 31 ) || ( m_port == PORT1 && m_bit > 21 ) )
		m_error = ERROR;

	if ( m_error == OK )
	{
		SYSCON->SYSAHBCLKCTRL0 |= ( 1 << CLOCK_GPIO0 ) | ( 1 << CLOCK_GPIO1 ) | ( 1 << CLOCK_IOCON );

		if ( m_direccion == OUTPUT )
		{
			SetPinModeOut();
			SetDirOutputs();
		}
		else
		{
			SetDirInputs();
			SetPinModeIn();
		}
	}
}

uint8_t Gpio::SetPin( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->SET[ m_port ] = 1 << m_bit;
		else
			GPIO->CLR[ m_port ] = 1 << m_bit;
	}
	return m_error;
}

uint8_t Gpio::ClrPin( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->CLR[ m_port ] = 1 << m_bit;
		else
			GPIO->SET[ m_port ] = 1 << m_bit;
	}
	return m_error;
}

uint8_t Gpio::SetTogglePin( void )
{
	if ( m_error == OK )
		GPIO->NOT[ m_port ] = 1 << m_bit;
	return m_error;
}

uint8_t Gpio::SetDirOutputs( void )
{
	if ( m_error == OK )
	{
		m_direccion = OUTPUT;
		GPIO->DIRSET[ m_port ] = 1 << m_bit;
	}
	return m_error;
}

uint8_t Gpio::SetDirInputs( void )
{
	if ( m_error == OK )
	{
		m_direccion = INPUT;
		GPIO->DIRCLR[ m_port ] = 1 << m_bit;
	}
	return m_error;
}

uint8_t Gpio::SetPinModeOut( void )
{
	if ( m_error == OK )
	{
		uint8_t i = IndiceIOCON( m_port , m_bit );
		if ( m_modo == OPENCOLECTOR )
			IOCON->PIO[ i ] |= IOCON_OD;
		else
			IOCON->PIO[ i ] &= ~IOCON_OD;
	}
	return m_error;
}

uint8_t Gpio::SetPinModeIn( void )
{
	if ( m_error == OK )
	{
		// Solo se tocan los bits MODE: el resto del registro queda como estaba
		uint8_t i = IndiceIOCON( m_port , m_bit );
		IOCON->PIO[ i ] = ( IOCON->PIO[ i ] & ~IOCON_MODE_MASK ) | ( m_modo << IOCON_MODE_SHIFT );
	}
	return m_error;
}

uint8_t Gpio::GetPin( void ) const
{
	if ( m_error != OK )
		return 0;

	uint8_t nivel = GPIO->B[ m_port ][ m_bit ];
	return ( m_actividad == HIGH ) ? nivel : !nivel;
}

Gpio& Gpio::operator=( bool estado )
{
	if ( estado )
		SetPin();
	else
		ClrPin();
	return *this;
}

Gpio::operator bool( )
{
	return GetPin();
}
