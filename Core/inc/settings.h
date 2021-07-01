/*
 * settings.h
 *
 *  Created on: 29 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_SETTINGS_H_
#define INC_SETTINGS_H_

#include "main.h"

struct MainSettings {
	uint16_t fpwm;
	uint16_t numberOfConversions;
	float	 currentConvCoef;
	float	 voltageConvCoef;
	float 	 bemfConvCoef;
};

void FillMainSettings(MainSettings& settings);

struct SixStepCommSettings {
	uint16_t fpwm;
	uint16_t pole_pairs;
	float	 startup_duty;
	uint16_t startup_rpm;
	uint16_t initialRPM;
	float 	 accelaration;
	uint16_t max_rpm;
	uint16_t desired_rpm;
	uint32_t rotor_align_t;
	uint32_t startup_ramp_t;
	uint32_t startup_sust_t;

	uint16_t rampMinRpm;
	uint16_t rampMaxRpm;
	uint16_t rampConstRpm;
	uint32_t rampTime;
};

void FillSixStepCommSettings(SixStepCommSettings& settings);


#endif /* INC_SETTINGS_H_ */
