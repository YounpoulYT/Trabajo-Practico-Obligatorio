/*******************************************************************************************************************************//**
 *
 * @file		Adc.h
 * @brief		Conversor analogico-digital (ADC) del LPC845: un objeto por canal
 * @author		Ing. Marcelo Trujillo (reescrito para el TPO)
 *
 * El ADC convierte una tension de 0 a 3,3 V en un numero de 0 a 4095 (12 bits).
 * Hay UN solo conversor, con 12 canales; cada canal esta atado a un pin fijo (ver pines.h).
 *
 * Como se usa:
 *   1. Crear un objeto Adc por canal (puede ser global; el constructor NO toca el hardware).
 *   2. Llamar UNA vez a Adc::Inicializar() despues de InicializarClock().
 *      Configura el ADC con todos los canales que se crearon.
 *   3. Llamar a Adc::Disparar() periodicamente (cada 1 ms, desde el SysTick).
 *      Cada disparo convierte TODOS los canales; al terminar salta una interrupcion que
 *      guarda los resultados.
 *   4. Leer cuando haga falta con GetResultado().
 *
 **********************************************************************************************************************************/

#ifndef ADC_H_
#define ADC_H_

#include "LPC845.h"

#if defined (__cplusplus)
extern "C" {
	void ADC_SEQA_IRQHandler( void );
}
#endif

class Adc
{
	public:
		enum filtro_t	{ CRUDO , PROMEDIO };

		static const uint8_t	CANTIDAD_CANALES = 12;
		static const uint8_t	MUESTRAS = 16;		// tamano del promedio movil
		static const int32_t	VALOR_MAXIMO = 4095;

		// canal: 0 a 11. filtro: CRUDO = ultima muestra, PROMEDIO = promedio de las ultimas MUESTRAS
		Adc( uint8_t canal , filtro_t filtro = PROMEDIO );

		static void Inicializar( void );		// una vez, despues de InicializarClock()
		static void Disparar( void );			// lanza una conversion de todos los canales

		int32_t GetResultado( void ) const;		// 0 (0 V) a 4095 (3,3 V)

		static void ManejarInterrupcion( void );	// la llama ADC_SEQA_IRQHandler

	private:
		uint8_t		m_canal;
		filtro_t	m_filtro;

		static uint16_t				s_canalesUsados;	// un bit por canal creado
		static volatile uint16_t	s_muestras[ CANTIDAD_CANALES ][ MUESTRAS ];
		static volatile uint8_t		s_indice[ CANTIDAD_CANALES ];

		static void Calibrar( void );
};

#endif /* ADC_H_ */
