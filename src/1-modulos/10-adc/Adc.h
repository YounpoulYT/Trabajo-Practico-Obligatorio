/*******************************************************************************************************************************//**
 *
 * @file		adc.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		9 nov. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef CLASES_I1_ADC_ADC_H_
#define CLASES_I1_ADC_ADC_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/

#include "systick.h"
#include "gpio.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/
#define DEMO_ADC_CLOCK_DIVIDER 1U
#define DEMO_ADC_SAMPLE_CHANNEL_NUMBER 0U
#define NRO_MUESTRAS 16

#define	ISE_ADC_SEQA			16

#define START					26
#define BURST					27
#define SINGLESTEP				28



#define	TAMANIO_BUFFER_ADC	16

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/
#if defined (__cplusplus)
	extern "C" {
	void ADC_SEQA_IRQHandler(void);
	void ADC_SEQB_IRQHandler(void) ;
	void ADC_THCMP_IRQHandler(void);
	void ADC_OVR_IRQHandler(void);
	}
#endif
/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLANTACION DE UNA CLASE
 **********************************************************************************************************************************/


class Adc
{
	private:
		uint32_t m_TipoDeFiltro ;
		uint8_t  m_Indice ;
		uint8_t  m_NumeroCanal;
		int32_t (*m_callback)(int32_t);
		static 	uint8_t m_inicializar;

	public:
		enum	resultado_t	{CRUDO,PROMEDIO,CALLBACK};
		Adc(uint8_t , resultado_t TipoDeFiltro );
		Adc(uint8_t , int32_t (*callback)(int32_t));

		virtual ~Adc();

		void start_conversion(void) ;
		uint32_t finished(uint16_t *channel_0, uint16_t *channel_1);
		void ADC_seqA_IRQHandler( void );
		void ADC_seqB_IRQHandler( void );
		void ADC_thcmp_IRQHandler( void );
		void ADC_ovr_IRQHandler( void ) ;
		int32_t getResultado();
		void setFiltro(uint32_t);

	private:
		uint32_t promedio( void );
		void seqa_set_mode_end_of_conversion(void);
		void seqa_set_mode_end_of_sequence(void);
		void set_clkdiv(uint8_t div) ;
		void seqa_set_channels(uint16_t channels);
		void seqa_enable_sequence(void) ;
		void inten_enable_seqa(void) ;
		void setBurstMode(void) ;
		void setSingleSequenceMode(void) ;
		void setStepMode(void) ;
		void calibrateADC(void);
		void inicializar(void);
};

#endif /* CLASES_I1_ADC_ADC_H_ */
