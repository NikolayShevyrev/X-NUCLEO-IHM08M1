/*
 * dma.h
 *
 *  Created on: 1 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_MEASUREMENT_H_
#define INC_MEASUREMENT_H_

#include "main.h"
#include "dfilter.h"
#include <array>

class Measurement {
private:
	std::array<uint16_t, 6> DCCurrent;
	std::array<uint16_t, 6> DCVoltage;
	std::array<uint16_t, 6> BEMF;

public:
	Measurement(){

	}

	void DMA_Init();

	float GetDCCurrent(void){
		float current = 0;
		for(int i = 0 ; i < CONVERSIONS_COUNT; i++){
			current += DCCurrent[i];
		}
		return (current * CURRENT_CONV_COEF);
	}

	float GetDCVoltage(void){
		static dFilter<float, 16> voltageFilter;
		float voltage = 0;
		for(int i = 0 ; i < CONVERSIONS_COUNT; i++){
			voltage += DCVoltage[i];
		}
		voltage = voltage * VOLTAGE_CONV_COEF;
		return voltageFilter.Calc(voltage);
	}

	float GetBEMF(void){
		float bemf = 0;
		for(int i = 0 ; i < CONVERSIONS_COUNT; i++){
			bemf += BEMF[i];
		}
		return (bemf * BEMF_CONV_COEF);
	}
};

#endif /* INC_MEASUREMENT_H_ */
