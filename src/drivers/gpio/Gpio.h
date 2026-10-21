/*******************************************************************************************************************************//**
 *
 * @file		Gpio.h
 * @brief		Pin digital de entrada o salida
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 * "Actividad": de que nivel se considera ACTIVO el pin.
 *   - HIGH: activo = 3,3 V  (SetPin pone 3,3 V; GetPin devuelve 1 con 3,3 V)
 *   - LOW:  activo = 0 V    (SetPin pone 0 V;   GetPin devuelve 1 con 0 V)
 * Ejemplo: un pulsador a GND con pull-up se crea con actividad LOW, asi GetPin() = 1 cuando esta apretado.
 *
 * El constructor YA configura el hardware (direccion y pull-up). Se puede usar como objeto global.
 *
 **********************************************************************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "LPC845.h"
#include "Inputs.h"
#include "Outputs.h"

class Gpio : public Outputs , public Inputs
{
	public:
		enum port_t			{ PORT0 , PORT1 };
		enum direction_t	{ INPUT , OUTPUT };
		enum power_t		{ OFF , ON };
		enum mode_output_t	{ PUSHPULL , OPENCOLECTOR };
		enum mode_input_t	{ INACTIVE , PULLDOWN , PULLUP , REPEATER };		// mismo orden que los bits MODE de IOCON
		enum activity_t		{ LOW , HIGH };
		enum error_t		{ ERROR = 2 , OK };

		// modo: PUSHPULL / OPENCOLECTOR si es salida, INACTIVE / PULLDOWN / PULLUP / REPEATER si es entrada
		Gpio( uint8_t port , uint8_t bit , uint8_t mode , uint8_t direction , uint8_t activity );

		uint8_t SetPin( void ) override;			// activa la salida
		uint8_t ClrPin( void ) override;			// desactiva la salida
		uint8_t SetTogglePin( void ) override;		// invierte la salida
		uint8_t SetDirOutputs( void ) override;
		uint8_t SetPinModeOut( void ) override;
		uint8_t SetDirInputs( void ) override;
		uint8_t SetPinModeIn( void ) override;
		uint8_t GetPin( void ) const override;		// 1 si el pin esta activo

		Gpio& operator=( bool estado );				// salida = Gpio::ON / Gpio::OFF
		explicit operator bool( );					// if ( entrada ) ...

		// Posicion del pin dentro de IOCON->PIO[]. NO sigue el orden de los pines (ver manual del LPC845).
		// La usa tambien el ADC para sacar el pull-up de sus pines.
		static uint8_t IndiceIOCON( uint8_t port , uint8_t bit );

	private:
		const uint8_t	m_port;
		const uint8_t	m_bit;
		const uint8_t	m_modo;
		uint8_t			m_direccion;
		const uint8_t	m_actividad;
		uint8_t			m_error;
};

#endif /* GPIO_H_ */
