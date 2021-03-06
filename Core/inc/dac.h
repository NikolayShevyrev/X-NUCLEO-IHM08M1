/*
 * dac.h
 *
 *  Created on: 26 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DAC_H_
#define INC_DAC_H_

#include "main.h"
#include "gpio.h"

class Dac {
private:
	uint16_t channel_;
public:
	Dac(uint16_t channel) :
		channel_(channel){
	}

	void Enable(){
		/* DAC Clock Enable */
		SET_BIT(RCC->APB1ENR, RCC_APB1ENR_DAC1EN);

		/* DAC channel_ pin init */
		GPIO_AnalogPinInit(DAC_1);
		GPIO_AnalogPinInit(DAC_2);

		/* DAC channel_ enable */
		SET_BIT(DAC->CR, (DAC_CR_EN1 << ((1 - 1)*16)));
		SET_BIT(DAC->CR, (DAC_CR_EN1 << ((2 - 1)*16)));
		//SET_BIT(DAC->CR, (DAC_CR_EN1 << ((channel_ - 1)*16)));
	}


};


#endif /* INC_DAC_H_ */
