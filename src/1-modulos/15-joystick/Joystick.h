/*******************************************************************************************************************************//**
 *
 * @file		Joystick.h
 * @brief		Joystick analogico de dos ejes (modulos HW-504 / KY-023)
 * @date		21 oct. 2026
 * @author		Juan Pablo Klymenko Campos
 *
 * Cada eje del joystick es un potenciometro de 10k entre +V y GND. El cursor (VRx / VRy)
 * entrega una tension proporcional a la posicion de la palanca:
 *
 *        0 V  ------------- 1,65 V ------------- 3,3 V
 *        0    ------------- ~2048  ------------- 4095     (cuentas del ADC de 12 bits)
 *     un extremo           palanca suelta           el otro extremo
 *
 * ATENCION: el pin "+5V" del modulo se conecta a 3,3 V. El ADC del LPC845 no admite mas de 3,3 V.
 *
 * Esta clase NO toca registros: usa dos objetos Adc (uno por eje) y traduce los numeros
 * a algo que le sirve al juego: una direccion (ARRIBA, ABAJO, IZQUIERDA, DERECHA o CENTRO).
 *
 **********************************************************************************************************************************/

#ifndef JOYSTICK_H_
#define JOYSTICK_H_

#include "Adc.h"

class Joystick
{
	public:
		enum direccion_t { CENTRO , ARRIBA , ABAJO , IZQUIERDA , DERECHA };

		// Mitad de la escala del ADC: donde deberia estar la palanca suelta
		static const int32_t CENTRO_TEORICO = 2048;

		// Cuanto hay que mover la palanca (en cuentas) para que cuente como movimiento.
		// Evita que el punto "tiemble" con la palanca suelta. ~15% de la escala.
		static const int32_t ZONA_MUERTA = 600;

		// invertirX / invertirY: dependen de como quede montado el joystick en la caja.
		// Si al mover a la derecha el punto va a la izquierda, se pone invertirX = true.
		Joystick( Adc *ejeX , Adc *ejeY , bool invertirX = false , bool invertirY = false );

		// Toma la posicion actual como "centro". Llamar con la palanca SUELTA.
		void Calibrar( void );

		// Posicion relativa al centro: aprox. -2048 ... +2047 (0 = palanca suelta)
		int32_t GetX( void );
		int32_t GetY( void );

		// Direccion dominante, o CENTRO si la palanca esta dentro de la zona muerta
		direccion_t GetDireccion( void );

	private:
		Adc		*m_ejeX;
		Adc		*m_ejeY;
		bool	m_invertirX;
		bool	m_invertirY;
		int32_t	m_centroX;
		int32_t	m_centroY;
};

#endif /* JOYSTICK_H_ */
