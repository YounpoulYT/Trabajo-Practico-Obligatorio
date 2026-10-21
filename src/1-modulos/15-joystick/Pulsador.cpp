/*******************************************************************************************************************************//**
 *
 * @file		Pulsador.cpp
 * @brief		Pulsador con antirrebote
 * @date		21 oct. 2026
 * @author		Juan Pablo Klymenko Campos
 *
 **********************************************************************************************************************************/

#include "Pulsador.h"

// Gpio( puerto , pin , modo , direccion , actividad )
// Actividad LOW: GetPin() devuelve 1 cuando el pin esta en 0 V, o sea cuando esta APRETADO.
Pulsador::Pulsador( uint8_t puerto , uint8_t pin ) :
	m_gpio( puerto , pin , Gpio::PULLUP , Gpio::INPUT , Gpio::LOW )
{
	m_estable = false;
	m_contador = 0;
	m_flanco = false;
}

void Pulsador::Actualizar( void )
{
	bool lectura = m_gpio.GetPin();

	if ( lectura == m_estable )
	{
		// Igual que lo que ya teniamos: no hay cambio en curso
		m_contador = 0;
		return;
	}

	// Distinto: puede ser un cambio real o un rebote. Esperamos a que se repita.
	m_contador++;
	if ( m_contador >= MUESTRAS )
	{
		m_estable = lectura;
		m_contador = 0;

		if ( m_estable )
			m_flanco = true;		// paso de suelto a apretado
	}
}

bool Pulsador::EstaPresionado( void ) const
{
	return m_estable;
}

bool Pulsador::FuePresionado( void )
{
	if ( m_flanco )
	{
		m_flanco = false;
		return true;
	}
	return false;
}
