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
#include "dac.h"
#include "settings.h"
#include "sixstepcomm.h"
#include "delay.h"
#include "TM1637.h"

void CheckButton(void);

/* Variables ---------------------------------------------------------*/
Timer1 timer1;
Timer6 timer6;
Timer3 timer3;
Timer4 timer4;

ADC_1 adc1;
ADC_2 adc2;
DMA1Channel1 dma1ch1;
DMA2Channel1 dma2ch1;

//Dac dac(2);
tm1637 display(I2C_CLK.port, I2C_CLK.pin, I2C_DIO.port, I2C_DIO.pin);

SixStepCommSettings motorSettings;

SixStepCommutation motor(&timer1, &timer4, &timer6);

state currentState = Stopped;

int main(){

	/* Configure the system clock */
	SystemClock_Config();

	/* Settings */
	FillSixStepCommSettings(motorSettings);

	/* Initialize all configured peripherals */
	GPIO_Init();
	timer1.Init(motorSettings.fpwm);
	timer4.Init();
	timer3.Init();
	timer6.Init();
	dma1ch1.Init();
	dma2ch1.Init();

	//dac.Enable();

	adc1.Init();
	adc2.Init();

	/* Init Motor */
	motor.Init(motorSettings);

	motor.SetDiraction(Clockwise);

	timer1.Start();
	timer1.PWMOutputsOn();

	display.initDisplay(BRIGHT);
	display.display(0x00, '-');
	display.display(0x01, 'r');
	display.display(0x02, 'D');
	display.display(0x03, 'Y');

	while(1){
		CheckButton();
	}
}

/**
  * @brief  Does stuff if user button was pressed
  * @retval None
  */
void CheckButton(void){
	if(!GPIO_ReadPin(Button)){
		DelayUS(100);
		while(!GPIO_ReadPin(Button)){
			DelayUS(100);
		}
		/* Do somesthing */
		if(currentState == Stopped) {
			currentState = Starting;
		}
		else {
			currentState = Stopping;
		}
		//GPIO_SetPin(LedX);
	}

}
