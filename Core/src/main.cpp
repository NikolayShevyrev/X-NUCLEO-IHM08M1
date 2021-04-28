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
#include "delay.h"

void CheckButton(void);

/* Variables ---------------------------------------------------------*/
Timer1 timer1;
Timer2 timer2;
Timer6 timer6;

ADC_1 adc1;
DMA1Channel1 dma1ch1;

SixStepCommSettings motorSettings;

SixStepCommutation motor(&timer1, &timer2, &timer6);

state currentState = Stopped;

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

	timer1.Start();
	timer1.PWMOutputsOn();

	while(1){
		CheckButton();
	}
}

/**
  * @brief  Does stuff if user button was pressed
  * @retval None
  */
void CheckButton(void){
	if(GPIO_ReadPin(Button)){
		DelayUS(10);
		while(GPIO_ReadPin(Button)){
			DelayUS(10);
		}
		/* Do somesthing */
		currentState = Starting;
		GPIO_SetPin(LedX);
	}

}
