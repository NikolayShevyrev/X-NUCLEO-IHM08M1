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
	settings.startup_duty 	= 0.8;
	settings.initialRPM		= 6;
	settings.accelaration	= 0.95; // Should be less then 1
	settings.startup_rpm 	= 60;
	settings.max_rpm 		= 150;
	settings.desired_rpm 	= 100;
	settings.rotor_align_t	= 50;
	settings.startup_ramp_t = 25000;
	settings.startup_sust_t = 100;
}
