/*
 * Timer.h
 *
 *  Created on: 22 may. 2024
 *      Author: Marcelo
 */

#ifndef SRC_3_FIRMWARE_TIMER_H_
#define SRC_3_FIRMWARE_TIMER_H_
#include "../04-PerifericosTemporizados/perifericotemporizado.h"
#include "tipos.h"

typedef void (*Timer_Callback)(void);

class Timer : public PerifericoTemporizado
{
	private :
		volatile uint32_t 	m_tmrRun;
		volatile bool 		m_tmrStandBy ;
		volatile bool 		m_tmrEvent;

		Timer_Callback 		m_tmrHandler ;
		volatile uint8_t  	m_tmrBase ;

	public:
		enum 	bases_t 	{ DEC , SEG , MIN };
		enum 	ticks_t 	{ DECIMAS = 100 , SEGUNDOS = 10 , MINUTOS  = 60 };
		enum 	standby_t 	{ RUN , PAUSE };

		static uint8_t	m_countTimer ;

		Timer();
		Timer( const bases_t base , const Timer_Callback callback = nullptr );

		void HandlerDelPeriferico( void );

		void TimerStart( uint32_t time, const Timer_Callback callback , const bases_t base );
		bool TimerStart( uint32_t time );
		bool SetTimer( uint32_t time );
		bool GetTimer( uint32_t &time );
		void StandByTimer( const standby_t accion );
		void TimerStop( void );
		bool operator==( uint32_t t );
		Timer& operator=( uint32_t time );
		explicit operator bool ();
		void TmrEvent ( void );
		~Timer();
	private:
		uint32_t BaseToticks( uint32_t time );
		uint32_t TicksToBase( uint32_t time ) const;

};

#endif /* SRC_3_FIRMWARE_TIMER_H_ */
