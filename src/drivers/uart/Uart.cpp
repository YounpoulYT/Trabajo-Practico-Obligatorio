/*******************************************************************************************************************************//**
 *
 * @file		Uart.cpp
 * @brief		UART con buffers circulares y transmision/recepcion por interrupcion
 * @author		Ing. Marcelo Trujillo (simplificado para el TPO)
 *
 * Cambios respecto de la catedra:
 *  - Los baudios se calculan con FREQ_CLOCK (antes estaba fijo en 48 MHz).
 *  - Se saco el modo LOOP (de prueba: conectaba la TX con la RX adentro del micro).
 *  - Transmitir(texto) ya no manda un '\0' al final.
 *  - Buffers de tamano fijo (sin new) y solo UART 0, 1 y 2.
 *  - El constructor solo guarda datos; el hardware se configura en Inicializar().
 *
 **********************************************************************************************************************************/

#include "Uart.h"
#include "swm.h"

#define	CLOCK_SWM			7				// bit de SYSAHBCLKCTRL0
#define	CLOCK_UART0			14				// UART0 = 14, UART1 = 15, UART2 = 16 (clock y reset usan el mismo bit)
#define	NVIC_UART0			3				// UART0 = 3, UART1 = 4, UART2 = 5
#define	FCLKSEL_MAIN_CLK	1				// el clock de la UART es el clock del sistema

#define	CFG_ENABLE			( 1 << 0 )
#define	CFG_8_BITS			( 1 << 2 )		// DATALEN = 1 -> 8 bits. Paridad y stop quedan en 0 (sin paridad, 1 stop)
#define	STAT_RXRDY			( 1 << 0 )		// llego un dato
#define	STAT_TXRDY			( 1 << 2 )		// se puede escribir otro dato
#define	INT_RXRDY			( 1 << 0 )
#define	INT_TXRDY			( 1 << 2 )

#define	OVERSAMPLING		16				// valor por defecto de OSR

static USART_Type * const USARTS[] = { USART0 , USART1 , USART2 };
static Uart *s_uarts[ 3 ] = { nullptr , nullptr , nullptr };

Uart::Uart( uint8_t numero , uint8_t puertoTx , uint8_t pinTx , uint8_t puertoRx , uint8_t pinRx , uint32_t baudios )
{
	m_numero = ( numero <= 2 ) ? numero : 0;
	m_puertoTx = puertoTx;
	m_pinTx = pinTx;
	m_puertoRx = puertoRx;
	m_pinRx = pinRx;
	m_baudios = baudios;
	m_usart = USARTS[ m_numero ];
	m_rxIn = m_rxOut = 0;
	m_txIn = m_txOut = 0;
}

bool Uart::Inicializar( void )
{
	if ( m_baudios == 0 )
		return false;

	uint32_t bit = 1 << ( CLOCK_UART0 + m_numero );

	// 1- Clock y reset de la UART
	SYSCON->SYSAHBCLKCTRL0 |= bit;
	SYSCON->PRESETCTRL0 &= ~bit;		// 0 = en reset
	SYSCON->PRESETCTRL0 |= bit;			// 1 = funcionando
	SYSCON->FCLKSEL[ m_numero ] = FCLKSEL_MAIN_CLK;

	// 2- Asignar los pines con la switch matrix (TX y RX se pueden poner en casi cualquier pin)
	SYSCON->SYSAHBCLKCTRL0 |= ( 1 << CLOCK_SWM );
	switch ( m_numero )
	{
		case 0:	PINASSIGN_Config( PA_U0_TXD , m_puertoTx , m_pinTx );	PINASSIGN_Config( PA_U0_RXD , m_puertoRx , m_pinRx );	break;
		case 1:	PINASSIGN_Config( PA_U1_TXD , m_puertoTx , m_pinTx );	PINASSIGN_Config( PA_U1_RXD , m_puertoRx , m_pinRx );	break;
		case 2:	PINASSIGN_Config( PA_U2_TXD , m_puertoTx , m_pinTx );	PINASSIGN_Config( PA_U2_RXD , m_puertoRx , m_pinRx );	break;
	}
	SYSCON->SYSAHBCLKCTRL0 &= ~( 1 << CLOCK_SWM );

	// 3- Formato y velocidad
	//    baudios = FREQ_CLOCK / ( OVERSAMPLING * ( BRG + 1 ) )  ->  BRG = FREQ_CLOCK / ( 16 * baudios ) - 1  (redondeado)
	m_usart->CFG = CFG_8_BITS;
	m_usart->BRG = ( ( FREQ_CLOCK + ( OVERSAMPLING * m_baudios ) / 2 ) / ( OVERSAMPLING * m_baudios ) ) - 1;

	// 4- Interrupciones: por ahora solo recepcion (la de transmision se prende cuando hay algo para mandar)
	s_uarts[ m_numero ] = this;
	m_usart->INTENSET = INT_RXRDY;
	NVIC->ISER[0] = ( 1 << ( NVIC_UART0 + m_numero ) );

	// 5- Habilitar
	m_usart->CFG |= CFG_ENABLE;
	return true;
}

void Uart::PonerEnTx( uint8_t dato )
{
	uint32_t siguiente = ( m_txIn + 1 ) % TAMANIO_BUFFER;

	// Si el buffer esta lleno, esperamos a que la interrupcion saque algo
	while ( siguiente == m_txOut )
	{
	}

	m_tx[ m_txIn ] = dato;
	m_txIn = siguiente;

	// Prender la interrupcion de TX: salta enseguida si la UART esta libre y empieza a vaciar el buffer
	m_usart->INTENSET = INT_TXRDY;
}

void Uart::Transmitir( const uint8_t *datos , uint32_t cantidad )
{
	for ( uint32_t i = 0 ; i < cantidad ; i++ )
		PonerEnTx( datos[ i ] );
}

void Uart::Transmitir( const char *texto )
{
	while ( *texto )
	{
		PonerEnTx( (uint8_t) *texto );
		texto++;
	}
}

bool Uart::Recibir( uint8_t &dato )
{
	if ( m_rxIn == m_rxOut )
		return false;				// no llego nada

	dato = m_rx[ m_rxOut ];
	m_rxOut = ( m_rxOut + 1 ) % TAMANIO_BUFFER;
	return true;
}

void Uart::ManejarInterrupcion( void )
{
	uint32_t estado = m_usart->INTSTAT;		// solo las interrupciones habilitadas

	// Llego un dato: guardarlo (si el buffer esta lleno, se pierde)
	if ( estado & STAT_RXRDY )
	{
		uint8_t dato = (uint8_t) m_usart->RXDAT;
		uint32_t siguiente = ( m_rxIn + 1 ) % TAMANIO_BUFFER;
		if ( siguiente != m_rxOut )
		{
			m_rx[ m_rxIn ] = dato;
			m_rxIn = siguiente;
		}
	}

	// La UART puede mandar otro dato: sacar el proximo del buffer, o apagar la interrupcion si no hay mas
	if ( estado & STAT_TXRDY )
	{
		if ( m_txOut != m_txIn )
		{
			m_usart->TXDAT = m_tx[ m_txOut ];
			m_txOut = ( m_txOut + 1 ) % TAMANIO_BUFFER;
		}
		else
			m_usart->INTENCLR = INT_TXRDY;
	}
}

void UART0_IRQHandler( void ) { if ( s_uarts[0] ) s_uarts[0]->ManejarInterrupcion(); }
void UART1_IRQHandler( void ) { if ( s_uarts[1] ) s_uarts[1]->ManejarInterrupcion(); }
void UART2_IRQHandler( void ) { if ( s_uarts[2] ) s_uarts[2]->ManejarInterrupcion(); }
