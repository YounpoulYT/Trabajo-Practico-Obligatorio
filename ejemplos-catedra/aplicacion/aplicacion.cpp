/*******************************************************************************************************************************//**
 *
 * @file		Aplicacion.cpp
 * @brief		Desrrollo de la consigna solicitada
 * @date		23/04/24
 * @author		Ing. Marcelo Trujillo
 *
 **********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** INCLUDES
 **********************************************************************************************************************************/
#include "inicializarInfotronic.h"
#include "aplicacion.h"

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

/***********************************************************************************************************************************
 *** OBJETOS GLOBALES PUBLICOS
 *********************************************************************************************************************************/

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES
 **********************************************************************************************************************************/
int32_t Programa[]={50,2,0,0,1,0};
Timer t_Temperatura , T_ProgTimeOut;;
bool fProgramar;

/***********************************************************************************************************************************
 *** VARIABLES GLOBALES DEL MODULO
 **********************************************************************************************************************************/

void Sheduller ( void )
{
	for ( uint8_t i = 0 ; i < PerifericoTemporizado::m_countPerifericosTemporizados  ; i ++ )
		g_perifericosTemporizados[i]->HandlerDelPeriferico();
}

uint8_t GetKey(void)
{
	t_Temperatura.TmrEvent();
	Control (  );
	return g_Teclado.GetKey();
}

void EvTemperatura ( void )
{
	t_Temperatura.TimerStart(5 , EvTemperatura, Timer::DEC);
	if ( !fProgramar )
		g_Display.SetDisplay(g_canal1.getResultado(), Display7Segmentos::DSP0);
}

void Programar( void )
{
	uint8_t c , indice = 0;
	fProgramar = true;
	while ( 1 )
	{
		switch ( indice )
		{
			case 0:
				c = Parametro ( indice + 1 , 10 , 1 ); // Banda
				break;
			case 1:
				c = Parametro ( indice + 1 , 1 , 0 );	// Power
				break;
			case 2:
				c = Parametro ( indice + 1 , 10 , 1 ); // Cero
				break;
			case 3:
				c = Parametro ( indice + 1 , 999 , 1 );	//Spam
				break;
			case 4:
				c = Parametro ( indice + 1 , 31 , 0 ); // Numero de equipo
				break;
		}
		switch ( c )
		{
			case F5:
				indice ++;
				indice %=5;
				break;
			case F2:
				g_Display.SetDisplay(REFERENCIA, Display7Segmentos::DSP1);
				return;
		}
	}
}

uint8_t Parametro ( uint8_t inx , int32_t Ls , int32_t Li )
{
	uint8_t c;
	g_Display.SetDisplay(inx , Display7Segmentos::DSP0);

	T_ProgTimeOut = 30;

	while ( 1 )
	{
		g_Display.SetDisplay(Programa[inx], Display7Segmentos::DSP1);

		while( ( c = GetKey( ) ) == NO_KEY )
		{
			if ( T_ProgTimeOut )
			{
				c = F2;
				break;
			}
		}

		switch ( c )
		{
			case F0:
				Programa[inx]++;
				if ( Programa[inx] > Ls)
					Programa[inx] = Ls;
				break;
			case F1:
				Programa[inx]--;
				if ( Programa[inx] < Li)
					Programa[inx] = Li;
				break;
			case F5:
				return F5;
			case F2:
				fProgramar = false;
				return F2;
		}
		T_ProgTimeOut = 30;
	}
}

void Control ( void  )
{
	if (g_canal1.getResultado() > REFERENCIA)
		g_out0 = Gpio::OFF;

	if (g_canal1.getResultado() <= REFERENCIA - BANDA)
		g_out0 = Gpio::ON;
}








