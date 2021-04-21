/*
 * main.cpp
 *
 *  Created on: Apr 6, 2021
 *      Author: Nikolay Shevyrev
 */

#include "main.h"
#include "measurement.h"
#include "gpio.h"
#include "timers.h"
#include "settings.h"
#include "sixstepcomm.h"
#include "motordrive.h"
#include "delay.h"


/* Variables ---------------------------------------------------------*/
Timer1 timer1;
Timer2 timer2;
Timer6 timer6;

Measurement meas;
SixStepCommSettings motorSettings;
MainSettings mainSettings;

SixStepCommutation motor(&timer1, &timer2, &timer6);

MotorDrive drive(new StoppedState);

int main(){

	/* Configure the system clock */
	SystemClock_Config();

	/* Settings */
	FillMainSettings(mainSettings);
	FillSixStepCommSettings(motorSettings);

	/* Init Motor */
	motor.Init(motorSettings, mainSettings);

	/* Initialize all configured peripherals */
	GPIO_Init();
	timer1.Init(mainSettings.fpwm);

	while(1){

	}
}
