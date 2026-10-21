/*******************************************************************************************************************************//**
 *
 * @file		Intext.h
 * @brief		Interrupcion externa por pin (PININT): llama a una funcion cuando un pin cambia
 * @author		Ing. Federico Bua (simplificado para el TPO)
 *
 * El LPC845 tiene 8 interrupciones de pin (0 a 7). Cada una se puede asociar a CUALQUIER pin.
 * Uso tipico: un boton que tiene que responder al instante, sin esperar a que el programa lo lea.
 *
 *   Intext boton( 0 , Gpio::PORT0 , 26 , FuncionQueAtiende );	// PININT0 en P0_26
 *   boton.set_flanco( Intext::DESCENDENTE );					// al apretar (3,3 V -> 0 V)
 *
 * La funcion que atiende se ejecuta DENTRO de la interrupcion: tiene que ser corta.
 * (Todavia no se usa en el TPO; queda lista para los botones.)
 *
 **********************************************************************************************************************************/

#ifndef INTEXT_H_
#define INTEXT_H_

#include "LPC845.h"

#if defined (__cplusplus)
extern "C" {
	void PININT0_IRQHandler( void );
	void PININT1_IRQHandler( void );
	void PININT2_IRQHandler( void );
	void PININT3_IRQHandler( void );
	void PININT4_IRQHandler( void );
	void PININT5_IRQHandler( void );
	void PININT6_IRQHandler( void );
	void PININT7_IRQHandler( void );
}
#endif

class Intext
{
	public:
		enum flanco_t	{ ASCENDENTE = 0 , DESCENDENTE = 1 , AMBOS_FLANCOS = 2 };
		enum nivel_t	{ BAJO = 0 , ALTO = 1 };

		// pint: numero de interrupcion (0..7). port / pin: el pin que se vigila.
		Intext( uint8_t pint , uint8_t port , uint8_t pin , void (*callback)( void ) );

		int set_flanco( flanco_t flanco );		// interrumpe en el flanco elegido
		int set_nivel( nivel_t nivel );			// interrumpe mientras el pin este en ese nivel
		void set_callback( void (*callback)( void ) );

		void Atender( void );					// lo llama el handler de la interrupcion

	private:
		uint8_t m_pint;
		uint8_t m_port;
		uint8_t m_pin;
		void (*m_callback)( void );

		void setup( void );
};

#endif /* INTEXT_H_ */
