/*
 * dac.cpp
 *
 *  Created on: Apr 12, 2023
 *      Author: fedebua
 */


#include "dac.h"

DAC::DAC(uint8_t dac_n)
{
	this->dac_n = dac_n;
	enabled = false;
}

int DAC::enable(void)
{
	int ret = 0;
	switch(dac_n){
	case 0:
		enable_dac0();
		dac = DAC0;
		break;
	case 1:
		enable_dac1();
		dac = DAC1;
		break;
	default:
		ret = -1;
	}
	return ret;
}

int DAC::disable(void)
{
	int ret = 0;
	switch(dac_n){
	case 0:
		disable_dac0();
		break;
	case 1:
		disable_dac1();
		break;
	default:
		ret = -1;
	}
	return ret;
}

DAC::~DAC()
{

}

void DAC::config_dac0_power(bool enable){
	if(enable)
		SYSCON->PDRUNCFG &= ~(1 << 13); //Prender el DAC
	else
		SYSCON->PDRUNCFG |= (1 << 13); //Apagar el DAC
}

void DAC::config_dac1_power(bool enable){
	if(enable)
		SYSCON->PDRUNCFG &= ~(1 << 14); //Prender el DAC
	else
		SYSCON->PDRUNCFG |= (1 << 14); //Apagar el DAC
}

void DAC::config_dac0_clock(bool enable){
	if(enable)
		SYSCON->SYSAHBCLKCTRL0 |= (1 << 27);
	else
		SYSCON->SYSAHBCLKCTRL0 &= ~(1 << 27);
}

void DAC::config_dac1_clock(bool enable){
	if(enable)
		SYSCON->SYSAHBCLKCTRL1 |= (1 << 1);
	else
		SYSCON->SYSAHBCLKCTRL1 &= ~(1 << 1);
}

void DAC::config_dac0_swm(bool enable){
	SYSCON->SYSAHBCLKCTRL0 |= (1 << 7); // 7 = SWM
	if(enable)
		SWM0->PINENABLE0 &= ~(1 << 26);
	else
		SWM0->PINENABLE0 |= (1 << 26);
	SYSCON->SYSAHBCLKCTRL0 &= ~(1 << 7); // 7 = SWM
}


void DAC::config_dac1_swm(bool enable){
	SYSCON->SYSAHBCLKCTRL0 |= (1 << 7); // 7 = SWM
	if(enable)
		SWM0->PINENABLE0 &= ~(1 << 27);
	else
		SWM0->PINENABLE0 |= (1 << 27);
	SYSCON->SYSAHBCLKCTRL0 &= ~(1 << 7); // 7 = SWM
}

void DAC::config_dac0_pinmode(bool enable){
	if(enable)
		IOCON->PIO[0] |= (1 << 16); //P0.17 DACMODE enable
	else
		IOCON->PIO[0] &= ~(1 << 16); //P0.17 DACMODE enable
}

void DAC::config_dac1_pinmode(bool enable){
	if(enable)
		IOCON->PIO[50] |= (1 << 16); //P0.29 DACMODE enable
	else
		IOCON->PIO[50] &= ~(1 << 16); //P0.29 DACMODE enable
}

void DAC::enable_dac0(void){
	config_dac0_power(true);
	config_dac0_clock(true);
	config_dac0_swm(true);
	config_dac0_pinmode(true);
	enabled = true;
}

void DAC::enable_dac1(void){
	config_dac1_power(true);
	config_dac1_clock(true);
	config_dac1_swm(true);
	config_dac1_pinmode(true);
	enabled = true;
}

void DAC::disable_dac0(void){
	config_dac0_pinmode(false);
	config_dac0_swm(false);
	config_dac0_clock(false);
	config_dac0_power(false);
	enabled = false;
}

void DAC::disable_dac1(void){
	config_dac1_pinmode(false);
	config_dac1_swm(false);
	config_dac1_clock(false);
	config_dac1_power(false);
	enabled = false;
}

void DAC::set_value(uint16_t value){
	value &= 0x3FF;
	dac->CR &= ~(0x3FF << 6);
	dac->CR |= (value << 6);
}
