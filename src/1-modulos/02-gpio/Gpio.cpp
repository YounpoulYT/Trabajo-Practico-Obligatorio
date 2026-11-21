/*******************************************************************************************************************************//**
 *
 * @file		gpio.cpp
 * @brief		Descripcion del modulo
 * @date		22 jun. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Gpio.h"

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
const uint8_t IOCON_INDEX_PIO0[] = { 17,11,6,5,4,3,16,15,4,13,8,7,2,1,18,10,9,0,30,29,28,27,26,25,24,23,22,21,20,0,0,35};
const uint8_t IOCON_INDEX_PIO1[] = { 36,37,3,41,42,43,46,49,31,32,55,54,33,34,39,40,44,45,47,48,52,53,0,0,0,0,0,0,0,50,51};

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
Gpio::Gpio ( uint8_t port , uint8_t bit , uint8_t mode , uint8_t direction , uint8_t activity) :
m_port ( port) , m_bit ( bit ) , m_modo ( mode ) , m_direccion ( direction ) , m_actividad ( activity )
{
	m_error = OK;

	if ( m_port > PORT1 )
		m_error = ERROR;
	if ( m_port == PORT0 && m_bit > B_PORT0 )
		m_error = ERROR;
	if ( m_port == PORT1 && m_bit > B_PORT1 )
		m_error = ERROR;

	if ( m_error == OK)
	{

		SYSCON->SYSAHBCLKCTRL0 |= (1 << 7);								// 7 = SWM
		SYSCON->SYSAHBCLKCTRL0 |= (1 << 6) | (1 << 20) | (1 << 18);		// 6 = GPIO0	20 = GPIO1	18 = IOCON

		if ( m_direccion == Gpio::OUTPUT )
		{
			SetPinModeOut ( );
			SetDirOutputs ( );
		}
		else
		{
			SetDirInputs ( );
			SetPinModeIn();
		}
	}

}

uint8_t Gpio::SetPin ( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->B[ m_port ][m_bit]  = ON;
		else
			GPIO->B[ m_port ][m_bit]  = OFF;
	}

	return m_error;
}

uint8_t Gpio::ClrPin ( void )
{
	if ( m_error == OK )
	{
		if ( m_actividad == HIGH )
			GPIO->CLR[ m_port ] = 1 << m_bit ;
		else
			GPIO->SET[ m_port ] = 1 << m_bit ;
	}
	return m_error;
}

uint8_t Gpio::SetTogglePin ( void )
{
	if ( m_error == OK )
		GPIO->NOT[ m_port ] = 1 << m_bit ;

	return m_error;
}

uint8_t Gpio::SetDirOutputs ( void )
{
	if ( m_error == OK )
	{
		m_direccion = OUTPUT ;
		GPIO->DIRSET[ m_port ] = 1 << m_bit ;
	}

	return m_error ;
}

uint8_t Gpio::SetDirInputs ( void )
{
	if ( m_error == OK )
	{
		m_direccion = INPUT ;
		GPIO->DIRCLR[ m_port ] = 1 << m_bit ;
	}

	return m_error ;
}

uint8_t Gpio::SetToggleDir ( void )
{
	if ( m_error == OK )
		GPIO->DIRNOT[m_port] = ( 1 << m_bit );

	return m_error;
}

uint8_t Gpio::GetPin ( void ) const
{
	uint8_t in;
	in = GPIO->B[m_port][m_bit] ;
	in = (m_actividad == HIGH ) ? in : !in;

	if ( m_error == OK )
		return in;
	return m_error;
}

uint8_t Gpio::SetPinModeOut( void )
{
	return 1;
}

uint8_t Gpio::SetPinModeIn ( void )
{
	uint8_t Indice_PortPin ;

	if ( m_error == OK )
	{
		Indice_PortPin = IOCON_INDEX_PIO0[m_bit];
		if ( m_port )
			Indice_PortPin = IOCON_INDEX_PIO1[m_bit];

		IOCON->PIO[ Indice_PortPin ] = ~0x18;
		IOCON->PIO[ Indice_PortPin ] = m_modo << 3;
	}
	return m_error;
}

Gpio & Gpio::operator=( bool estado )
{
	if ( estado == ON )
		SetPin();
	else
		ClrPin();
	return *this;
}

bool Gpio::operator== ( bool estado )
{
	if ( GetPin() == estado )
		return true ;
	return false;
}

bool Gpio::operator== ( Gpio & gpio )
{
	if ( GetPin() == gpio.GetPin() )
		return true ;
	return false;
}
Gpio::operator bool( )
{
	return GetPin();
}
