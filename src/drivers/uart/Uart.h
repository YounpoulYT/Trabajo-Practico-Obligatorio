/*******************************************************************************************************************************//**
 *
 * @file		Uart.h
 * @brief		UART con buffers circulares y transmision/recepcion por interrupcion
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 * Formato fijo: 8 bits de datos, sin paridad, 1 bit de stop (8N1).
 *
 * Transmitir() NO espera a que salgan los bytes: los deja en un buffer y la interrupcion
 * los va mandando de a uno. Asi el juego no se frena mientras se envia.
 * Lo que llega se guarda en otro buffer y se saca con Recibir().
 *
 **********************************************************************************************************************************/

#ifndef UART_H_
#define UART_H_

#include "LPC845.h"
#include "ComunicacionAsincronica.h"

#if defined (__cplusplus)
extern "C" {
	void UART0_IRQHandler( void );
	void UART1_IRQHandler( void );
	void UART2_IRQHandler( void );
}
#endif

class Uart : public ComunicacionAsincronica
{
	public:
		static const uint32_t TAMANIO_BUFFER = 128;

		// numero: UART 0, 1 o 2. El constructor NO toca el hardware.
		Uart( uint8_t numero , uint8_t puertoTx , uint8_t pinTx , uint8_t puertoRx , uint8_t pinRx , uint32_t baudios );

		bool Inicializar( void );				// despues de InicializarClock()

		void Transmitir( const char *texto ) override;
		void Transmitir( const uint8_t *datos , uint32_t cantidad ) override;
		bool Recibir( uint8_t &dato ) override;

		void ManejarInterrupcion( void );		// la llama UARTn_IRQHandler

	private:
		uint8_t		m_numero;
		uint8_t		m_puertoTx , m_pinTx , m_puertoRx , m_pinRx;
		uint32_t	m_baudios;
		USART_Type	*m_usart;

		// Buffers circulares: "In" es donde se escribe, "Out" es donde se lee. Si son iguales, esta vacio.
		volatile uint8_t	m_rx[ TAMANIO_BUFFER ];
		volatile uint32_t	m_rxIn , m_rxOut;
		volatile uint8_t	m_tx[ TAMANIO_BUFFER ];
		volatile uint32_t	m_txIn , m_txOut;

		void PonerEnTx( uint8_t dato );
};

#endif /* UART_H_ */
