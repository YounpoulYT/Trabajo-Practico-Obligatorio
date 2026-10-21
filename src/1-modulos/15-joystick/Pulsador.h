/*******************************************************************************************************************************//**
 *
 * @file		Pulsador.h
 * @brief		Pulsador a GND con pull-up interno, antirrebote y deteccion de "recien apretado"
 * @date		21 oct. 2026
 * @author		Juan Pablo Klymenko Campos
 *
 * Conexion: un terminal del pulsador al pin, el otro a GND. No hace falta resistencia:
 * se usa el pull-up interno del LPC845. Suelto = 3,3 V (1), apretado = 0 V (0).
 * Sirve tambien para el boton SW de los joysticks (se conecta igual).
 *
 * Antirrebote: los contactos mecanicos "rebotan" unos milisegundos al cerrarse. Por eso
 * solo se acepta un cambio cuando la lectura se repite MUESTRAS veces seguidas.
 * Actualizar() se tiene que llamar a intervalos regulares (por ejemplo cada 10 ms).
 *
 **********************************************************************************************************************************/

#ifndef PULSADOR_H_
#define PULSADOR_H_

#include "Gpio.h"

class Pulsador
{
	public:
		Pulsador( uint8_t puerto , uint8_t pin );

		// Leer el pin y filtrar rebotes. Llamar cada ~10 ms.
		void Actualizar( void );

		// true mientras el boton este apretado (ya filtrado)
		bool EstaPresionado( void ) const;

		// true UNA sola vez por cada apretada (flanco). Para acciones como "cambiar de modo".
		bool FuePresionado( void );

	private:
		static const uint8_t MUESTRAS = 3;		// 3 lecturas iguales x 10 ms = 30 ms estable

		Gpio	m_gpio;
		bool	m_estable;			// ultimo estado aceptado
		uint8_t	m_contador;			// cuantas lecturas seguidas distintas de m_estable
		bool	m_flanco;			// se apreto y todavia nadie lo leyo
};

#endif /* PULSADOR_H_ */
