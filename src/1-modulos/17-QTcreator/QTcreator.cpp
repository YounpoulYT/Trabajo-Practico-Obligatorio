/*
 * QTcreator.cpp
 *
 *  Created on: 27 nov. 2026
 *      Author: Juanp
 */

/**********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include <17-QTcreator/QTcreator.h>
/***********************************************************************************************************************************
 *** FUNCIONES AUXILIARES
 **********************************************************************************************************************************/

void QTcreator::AgregarNumero( uint16_t valor , uint8_t posicion )
{
	//Si se pasa lo clavamos en 999
	if ( valor > 999 )
		valor = 0;

	//Separar en dígitos y pasarlos a carácter ASCll
	m_trama[ posicion ]     = ( valor / 100 ) + '0';			// centenas
	m_trama[ posicion + 1 ] = ( ( valor / 10 ) % 10 ) + '0';	// decenas
	m_trama[ posicion + 2 ] = ( valor % 10 ) + '0';				// unidades
}

/***********************************************************************************************************************************
 *** FUNCIONES MIEMBRO
 **********************************************************************************************************************************/

QTcreator::QTcreator (ComunicacionAsincronica *uart)
{
	m_uart = uart;
}

void QTcreator::EnviarMenu(uint8_t opcion)
{

	m_trama[0] = '#';

	m_trama[1] = 'M';

	m_trama[2] = opcion + '0';

	m_trama[3] = '$';

	m_trama[4] = '\r';

	m_trama[5] = '\n';

	m_uart->Transmit( m_trama , 6 ); //manda los caracteres para que qtcreator los reciba

}

void QTcreator::EnviarSerpiente( uint8_t estado , uint16_t puntaje , uint16_t manzanas , uint16_t tiempo )
{

	m_trama[0] = '#';

	m_trama[1] = 'S';

	m_trama[2] = estado + '0';

	AgregarNumero(puntaje,3); //3 a 5

	AgregarNumero(manzanas,6); //6 a 8

	AgregarNumero(tiempo,9); //9 a 11

	m_trama[12] = '$';

	m_trama[13] = '\r';

	m_trama[14] = '\n';
	m_uart->Transmit( m_trama , 15 );

}


