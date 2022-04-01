/*
 * settings.cpp
 *
 *  Created on: 29 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#include "settings.h"

void FillMainSettings(MainSettings& settings){
	settings.fpwm 					= 20000;
	settings.numberOfConversions 	= 6;
	settings.currentConvCoef 		= (float)(0.001F / (float)settings.numberOfConversions);
	settings.voltageConvCoef 		= (float)(0.01F / (float)settings.numberOfConversions);
	settings.bemfConvCoef 			= (float)(0.01F / (float)settings.numberOfConversions);
}

void FillSixStepCommSettings(SixStepCommSettings& settings){
	settings.fpwm 			= 20000;
	settings.pole_pairs 	= 12;
	settings.startup_duty 	= 0.32;
	settings.initialRPM		= 6;
	settings.accelaration	= 0.99; // Should be less then 1
	settings.startup_rpm 	= 25;
	settings.max_rpm 		= 200;
	settings.desired_rpm 	= 50;
	settings.rotor_align_t	= 150;

	//RPM Ramp
	settings.rampMinRpm		= 70;
	settings.rampMaxRpm		= 120;
	settings.rampConstRpm	= 70;
	settings.rampTime		= 420; // In seconds

}
