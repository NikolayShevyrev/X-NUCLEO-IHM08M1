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
	settings.pole_pairs 	= 4;
	settings.startup_duty 	= 0.2;
	settings.initialRPM		= 300;
	settings.accelaration	= 0.99; // Should be less then 1
	settings.startup_rpm 	= 500;
	settings.max_rpm 		= 3000;
	settings.desired_rpm 	= 1000;
	settings.rotor_align_t	= 100;
	//settings.startup_ramp_t = 25000;
	//settings.startup_sust_t = 100;
}
