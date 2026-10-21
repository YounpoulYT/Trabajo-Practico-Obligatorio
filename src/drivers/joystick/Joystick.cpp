/*******************************************************************************************************************************//**
 *
 * @file		Joystick.cpp
 * @brief		Joystick analogico de dos ejes
 * @date		21 oct. 2026
 * @author		Juan Pablo Klymenko Campos
 *
 **********************************************************************************************************************************/

#include "Joystick.h"

Joystick::Joystick( Adc *ejeX , Adc *ejeY , bool invertirX , bool invertirY )
{
	m_ejeX = ejeX;
	m_ejeY = ejeY;
	m_invertirX = invertirX;
	m_invertirY = invertirY;

	// Hasta que se llame a Calibrar() suponemos el centro ideal
	m_centroX = CENTRO_TEORICO;
	m_centroY = CENTRO_TEORICO;
}

void Joystick::Calibrar( void )
{
	// Los potenciometros baratos no quedan exactamente en la mitad: medimos donde queda de verdad
	m_centroX = m_ejeX->GetResultado();
	m_centroY = m_ejeY->GetResultado();
}

int32_t Joystick::GetX( void )
{
	int32_t x = m_ejeX->GetResultado() - m_centroX;
	return m_invertirX ? -x : x;
}

int32_t Joystick::GetY( void )
{
	int32_t y = m_ejeY->GetResultado() - m_centroY;
	return m_invertirY ? -y : y;
}

Joystick::direccion_t Joystick::GetDireccion( void )
{
	int32_t x = GetX();
	int32_t y = GetY();

	int32_t absX = ( x < 0 ) ? -x : x;
	int32_t absY = ( y < 0 ) ? -y : y;

	// Palanca suelta (o casi): no hay movimiento
	if ( absX < ZONA_MUERTA && absY < ZONA_MUERTA )
		return CENTRO;

	// Para juegos como Snake o Pong alcanza con 4 direcciones:
	// gana el eje que este mas lejos del centro
	if ( absX >= absY )
		return ( x > 0 ) ? DERECHA : IZQUIERDA;

	return ( y > 0 ) ? ARRIBA : ABAJO;
}
