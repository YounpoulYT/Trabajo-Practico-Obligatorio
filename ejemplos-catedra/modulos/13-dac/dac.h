/*
 * dac.h
 *
 *  Created on: Apr 12, 2023
 *      Author: fedebua
 */

#ifndef DAC_H_
#define DAC_H_

#include "LPC845.h"


class DAC {
public:
	DAC(uint8_t dac_n);
	virtual ~DAC();
	int enable(void);
	int disable(void);
	void set_value(uint16_t value);

private:
	bool enabled;
	uint8_t dac_n;
	DAC_Type* dac;

	void config_dac0_power(bool enable);
	void config_dac1_power(bool enable);
	void config_dac0_clock(bool enable);
	void config_dac1_clock(bool enable);
	void config_dac0_swm(bool enable);
	void config_dac1_swm(bool enable);
	void config_dac0_pinmode(bool enable);
	void config_dac1_pinmode(bool enable);
	void enable_dac0(void);
	void enable_dac1(void);
	void disable_dac0(void);
	void disable_dac1(void);
};




#endif /* DAC_H_ */
