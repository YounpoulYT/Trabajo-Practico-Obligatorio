/*******************************************************************************************************************************//**
 *
 * @file		adc.cpp
 * @brief		Descripcion del modulo
 * @date		9 nov. 2022
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "Adc.h"

/* Definicion de clocks */
#define CLOCK_SWM				7
#define CLOCK_ADC				24
#define CLOCK_DAC1				1

/* Definicion de Pines */
#define CANAL_0_ADC				14
#define PIN_DAC_1				27

/* Definicion de Power */
#define POWER_ADC				4
#define POWER_DAC1				14

/* Definicion bits ADC */
#define CALMODE					30
#define CANAL_0					0
#define BURST					27
#define	SEQ_A_ENABLE			31

#define	INT_SEQ_A				0

#define	FLAG_INTERRUPT_SEQ_A	28

/* Definicion bits DAC */
#define ADC1_BIAS				16

extern uint32_t cuentasADC;
/***********************************************************************************************************************************
 *** DEFINES PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** MACROS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TIPOS DE DATOS PRIVADOS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** TABLAS PRIVADAS AL MODULO
 **********************************************************************************************************************************/

static uint32_t g_result[16][TAMANIO_BUFFER_ADC] ;
struct canal
{
	uint16_t Indice;
	bool enable;
};

struct canal CanalesHabilitados[] =
{
		{ 0 , false },	//canal 0  :port0,   7
		{ 0 , true },	//canal 1  :port0,   6
		{ 0 , false },	//canal 2  :port0,  14
		{ 0 , false },	//canal 3  :port0,  23
		{ 0 , false },	//canal 4  :port0,  22
		{ 0 , false },	//canal 5  :port0,  21
		{ 0 , false },	//canal 6  :port0,  20
		{ 0 , false },	//canal 7  :port0,  19
		{ 0 , false },	//canal 8  :port0,  18
		{ 0 , false },	//canal 9  :port0,  17
		{ 0 , false },	//canal 10 :port0,  13
		{ 0 , false }	//canal 11 :port0,   4
};

Adc	*g_adc[ 4 ] = { nullptr , nullptr , nullptr ,nullptr };

uint8_t Adc::m_inicializar = 0 ;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PUBLICAS
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES PRIVADAS AL MODULO
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** IMPLEMENTACION DE LOS METODODS DE LA CLASE
 **********************************************************************************************************************************/
uint32_t m_result_0;
uint32_t m_result_1;
uint32_t m_finished;

Adc::Adc(uint8_t canal ,  resultado_t TipoDeFiltro )
{
	m_callback = nullptr;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = TipoDeFiltro ;
	inicializar();
}
Adc::Adc(uint8_t canal ,  int32_t (*callback)(int32_t) )
{
	m_callback = callback;
	m_Indice = 0;
	m_NumeroCanal = canal ;
	m_TipoDeFiltro = CALLBACK ;
	inicializar();
}
void Adc::inicializar(void)
{

	if ( !m_inicializar )
	{
		m_inicializar = true;
		// Primero habilitamos alimentacion del periferico
		SYSCON->PDRUNCFG &= ~SYSCON_PDRUNCFG_ADC_MASK;

		// Habilitamos clock del periferico
		SYSCON->SYSAHBCLKCTRL0 |= SYSCON_SYSAHBCLKCTRL0_ADC_MASK;

		// 2-Setea divisor de ADC en 1
		//	SYSCON->ADCCLKDIV |= (uint32_t)(0x01);

		SYSCON->SYSAHBCLKCTRL0 |= (1 << 7) ;				//swm_power_on();

		for ( uint8_t i = 0 ; i < (sizeof( CanalesHabilitados) / sizeof( struct canal) ); i++)
		{
			if ( CanalesHabilitados[i].enable )
				SWM0->PINENABLE0 &= ~SWM_PINENABLE0_ADC_MASK( i ); 	//registro activo bajo;
		}

		SYSCON->SYSAHBCLKCTRL0 &= ~(1 << 7);				//swm_power_off();

		calibrateADC( );

		// Seleccionamos divisor del clock a nivel del periferico en 1

		// NOTA 1: Si bien se coloca cero en el divisor, recordar que la division es
		// por ese valor +1

		// NOTA 2: Con el FRO a 12MHz, y ambos divisores (SYSCON y periferico) en 1, la
		// frecuencia de conversion obtenida es de 12MHz / 25 = 480KHz. Con lo que una
		// conversion tomara ~2.08usec

		// Modo Sincrónico de clock
		ADC0->CTRL &= ~(uint32_t)ADC_CTRL_ASYNMODE_MASK;

		set_clkdiv(12);

		// Modo Low Power off
		ADC0->CTRL &= ~(uint32_t)ADC_CTRL_LPWRMODE_MASK;

		// Configuramos secuencia de conversion A para canal 0
		for ( uint8_t i = 0 ; i < (sizeof( CanalesHabilitados) / sizeof( struct canal) ); i++)
		{
			if ( CanalesHabilitados[i].enable )
				seqa_set_channels(i);
		}

		// Configuramos modo para que la interrupcion se efectue al finalizar toda la secuencia
		//seqa_set_mode_end_of_sequence();
		seqa_set_mode_end_of_conversion();
		// Habilitamos secuencia de conversion A
		seqa_enable_sequence();

		// Habilitamos interrupciones para fin de conversion de secuencia A
		inten_enable_seqa();

		setStepMode() ;

		g_adc[ 0 ] = this ;

		NVIC->ISER[0] |= (1 << ISE_ADC_SEQA);

		start_conversion( );
	}
}

Adc::~Adc()
{
	// TODO Auto-generated destructor stub
}
void Adc::seqa_set_mode_end_of_conversion(void)
{
	ADC0->SEQ_CTRL[0] &= ~(1 << 30);//SEQA_CTR
}

void Adc::seqa_set_mode_end_of_sequence(void)
{
	ADC0->SEQ_CTRL[0] |= (1 << 30);	//SEQA_CTR
}

void Adc::set_clkdiv(uint8_t div)
{
	ADC0->CTRL &= ~0x000000FF;	//SEQA_CTRL
	ADC0->CTRL |= div;			//SEQA_CTRL
}

void Adc::seqa_set_channels(uint16_t channel)
{
	uint32_t channels = 0;

	channels = 1 << channel ;
	ADC0->SEQ_CTRL[0] |= (channels & 0x03FF) ; 	//SEQA_CTRL
}

void Adc::seqa_enable_sequence(void)
{
	ADC0->SEQ_CTRL[0] |= (1 << 31) ; //SEQA_CTRL
}

void Adc::inten_enable_seqa(void)
{
	ADC0->INTEN |= (1 << INT_SEQ_A) ;
}


void Adc::start_conversion(void)
{
	ADC0->SEQ_CTRL[0] |= (1 << 26) ; //SEQA_CTRL
}

uint32_t Adc::finished(uint16_t *channel_0, uint16_t *channel_1)
{
	// NOTA: Guardo el valor actual del NVIC_ISER dado que de no hacerlo, por alguna razon
	// que al momento no logro descifrar, tambien inhabilita las interrupciones del periferico
	// USART pero luego no vuelve a habilitarlas
	uint32_t ret = 0;
	uint32_t aux ;

	aux = NVIC->ISER[0] ;
	NVIC->ICER[0] |= (1 << 16);

	if (m_finished)
	{
		m_finished = 0;
		ret = 1;
	}
	NVIC->ISER[0] |= ( aux | (1 << 16) );

	return ret;
}

void Adc::setBurstMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<START);
	ADC0->SEQ_CTRL[0] &= ~(1<<SINGLESTEP);
	ADC0->SEQ_CTRL[0] |= (1<<BURST);
}

void Adc::setSingleSequenceMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<SINGLESTEP);
	ADC0->SEQ_CTRL[0] &= ~(1<<BURST);
	ADC0->SEQ_CTRL[0] |= (1<<START);
}

void Adc::setStepMode(void) {
	ADC0->SEQ_CTRL[0] &= ~(1<<BURST);
	ADC0->SEQ_CTRL[0] &= ~(1<<START);
	ADC0->SEQ_CTRL[0] |= (1<<SINGLESTEP);
}

		
void Adc::ADC_seqA_IRQHandler(void)
{
	uint32_t resultado;
	uint8_t canal ;

	resultado =  ADC0->SEQ_GDAT[0];
	canal = ( resultado >> 26 ) & 0xf;

	switch ( m_TipoDeFiltro )
	{
		case CRUDO:
			g_result[ canal ][ 0 ] = ((resultado >> 4) & 0xFFF);
			break;
		case PROMEDIO:
		case CALLBACK:

			g_result[ canal ][CanalesHabilitados[canal].Indice] = ((resultado >> 4) & 0xFFF);

			break;
	}
	CanalesHabilitados[canal].Indice ++;
	CanalesHabilitados[canal].Indice %= TAMANIO_BUFFER_ADC;
	start_conversion( );

	// Esta interrupcion va a ser generada cuando se haya convertido una secuencia completa de los dos canales
	// Para limpiar el flag de interrupcion en estos casos, hay que escribir un 1 en SEQA_INT del registro FLAGS
	//ADC0->FLAGS |= (1 << 28);
	m_finished = 1;
	// El resultado se encuentra en los bits 15:4 de los registros, pero esa conversion la haremos en la funcion adc_finished
}
void Adc::ADC_seqB_IRQHandler(void)
{
	return ;
}
void Adc::ADC_thcmp_IRQHandler(void)
{
	return ;
}
void Adc::ADC_ovr_IRQHandler(void)
{
	return ;
}

void Adc::calibrateADC(void){

	uint32_t temporal;
    uint32_t frecuencia = 0U;

	// 4-Calibra ADC
	//a-Se debe obtener la frecuencia del ADC
	frecuencia = FREQ_CLOCK;
	temporal = ADC0->CTRL;
	//b-Se debe llevar la frecuencia del ADC a 500kHz
	ADC0->CTRL &= ~ADC_CTRL_CLKDIV_MASK;
	ADC0->CTRL |= ADC_CTRL_CLKDIV((frecuencia / 500000U) - 1U);
	/* Clear the LPWR bit. */
	ADC0->CTRL &= ~ADC_CTRL_LPWRMODE_MASK;

	/* Start ADC self-calibration. */
	ADC0->CTRL |= ADC_CTRL_CALMODE_MASK;

	while( ADC0->CTRL &= 0x40000000) {
		// No hago nada hasta que no termine la calibracion del ADC.
	}; 

	/* Restore the contents of the ADC CTRL register. */
	ADC0->CTRL = temporal;
}

void Adc::setFiltro (uint32_t tipo) {

	switch (tipo) {

		case PROMEDIO:
			m_TipoDeFiltro = PROMEDIO;
			break;

		case CRUDO:
		default:
			m_TipoDeFiltro = CRUDO;
			break;
	}
}

void ADC_SEQA_IRQHandler(void)
{
	if ( g_adc[0] )
		g_adc[0]->ADC_seqA_IRQHandler( );
}

void ADC_SEQB_IRQHandler(void)
{
	if ( g_adc[1] )
		g_adc[1]->ADC_seqB_IRQHandler( );

}
void ADC_THCMP_IRQHandler(void)
{
	if ( g_adc[2] )
		g_adc[2]->ADC_thcmp_IRQHandler( );

}
void ADC_OVR_IRQHandler(void)
{
	if ( g_adc[3] )
		g_adc[3]->ADC_ovr_IRQHandler( );

}

int32_t Adc::getResultado(void) {

	uint32_t resultadoParcial;

	switch ( m_TipoDeFiltro )
	{
		case CRUDO :
			resultadoParcial = g_result[m_NumeroCanal][0];
			break;
		case PROMEDIO:
			resultadoParcial = promedio( );
			break;
		case CALLBACK:
			resultadoParcial = promedio( );
			resultadoParcial = m_callback ( resultadoParcial );
			break;
	}

	return resultadoParcial;
}

uint32_t Adc::promedio( void )
{
	uint32_t resultadoParcial = 0;
	for (uint32_t i=0 ; i < TAMANIO_BUFFER_ADC ; i++)
		resultadoParcial += g_result[m_NumeroCanal][i];
	resultadoParcial /= TAMANIO_BUFFER_ADC;
	return resultadoParcial ;
}

