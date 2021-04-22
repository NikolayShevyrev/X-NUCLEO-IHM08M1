/*
 * main.cpp
 *
 *  Created on: Apr 6, 2021
 *      Author: Nikolay Shevyrev
 */

#include "main.h"
#include "gpio.h"
#include "timers.h"
#include "adc.h"
#include "dma.h"
#include "settings.h"
#include "sixstepcomm.h"
#include "motordrive.h"
#include "delay.h"


/* Variables ---------------------------------------------------------*/
Timer1 timer1;
Timer2 timer2;
Timer6 timer6;

ADC_1 adc1;
DMA1Channel1 dma1ch1;

SixStepCommSettings motorSettings;

SixStepCommutation motor(&timer1, &timer2, &timer6);

MotorDrive drive(new StoppedState);

int main(){

	/* Configure the system clock */
	SystemClock_Config();

	/* Settings */
	FillSixStepCommSettings(motorSettings);

	/* Init Motor */
	motor.Init(motorSettings);

	/* Initialize all configured peripherals */
	GPIO_Init();
	timer1.Init(motorSettings.fpwm);
	adc1.Init();
	dma1ch1.Init();

	while(1){

	}
}
