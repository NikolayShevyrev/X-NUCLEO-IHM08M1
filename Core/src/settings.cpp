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
	settings.pole_pairs 	= 4;//12;
	settings.startup_duty 	= 0.2;//0.32;
	settings.initialRPM		= 150;//6;
	settings.accelaration	= 0.99; // Should be less then 1
	settings.startup_rpm 	= 250;//25;
	settings.max_rpm 		= 3000;//150;
	settings.desired_rpm 	= 1000;//50;
	settings.rotor_align_t	= 150;

	//RPM Ramp
	settings.rampMinRpm		= 1000;//50;
	settings.rampMaxRpm		= 3000;//150;
	settings.rampConstRpm	= 1000;//50;
	settings.rampTime		= 15;//600; // In seconds

}
