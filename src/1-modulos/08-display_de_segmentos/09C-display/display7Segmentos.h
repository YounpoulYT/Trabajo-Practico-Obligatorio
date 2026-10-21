/*******************************************************************************************************************************//**
 *
 * @file		display.h
 * @brief		Breve descripción del objetivo del Módulo
 * @date		26 jul. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MODULO
 **********************************************************************************************************************************/

#ifndef SRC_I0_DISPLAY7SEGMENTOS_DISPLAY_H_
#define SRC_I0_DISPLAY7SEGMENTOS_DISPLAY_H_

/***********************************************************************************************************************************
 *** INCLUDES GLOBALES
 **********************************************************************************************************************************/
#include <1-modulos/08-display_de_segmentos/09A-barrido/barrido-interfaz/Barrido.h>
#include <1-modulos/08-display_de_segmentos/09A-barrido/I4017Barrido.h>
#include <1-modulos/08-display_de_segmentos/09B-segmentos/I4511Segmentos.h>
#include <1-modulos/08-display_de_segmentos/09C-display/gruposdedigitos.h>
#include "Gpio.h"

#include "systick.h"
#include "perifericotemporizado.h"

/***********************************************************************************************************************************
 *** DEFINES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPO DE DATOS GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLANTACION DE UNA CLASE
 **********************************************************************************************************************************/
class Display7Segmentos : public PerifericoTemporizado
{
	private:
		Segmentos  *m_segmentos;
		Barrido  *m_barrido;
		struct GrupoDeDigitos *m_GruposDeDigitos;
		uint8_t * m_pIndiceCorrector;
		uint8_t m_ticks;
		uint8_t m_CantiadDeDigitos;
		uint8_t m_IndiceBarrido;
		uint8_t *m_BufferDisplay;

		uint8_t *m_aux;
	public:
		enum nDisplay { DSP0,DSP1,DSP2,DSP3,DSP4,DSP5 };
		Display7Segmentos(
			Segmentos * segmentos , Barrido *barrido,
			struct GrupoDeDigitos *GruposDeDigitos,
			uint8_t *pIndiceCorrector = nullptr);
		void HandlerDelPeriferico (void);
		void SetDisplay( uint32_t valor , nDisplay dsp );
		virtual ~Display7Segmentos();
};

#endif /* SRC_I0_DISPLAY7SEGMENTOS_DISPLAY_H_ */
