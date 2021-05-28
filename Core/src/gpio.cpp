/*
 * gpio.cpp
 *
 *  Created on: 16 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */


#include "main.h"
#include "gpio.h"

/* GPIO Pins ---------------------------------------------------------*/

// LEDs
const gpio_t LedX(GPIOB, 2);
const gpio_t DebugPin_1(GPIOC, 8);

// Buttons
const gpio_t Button(GPIOC, 13);

// PWM Outputs
const gpio_t UH(GPIOA, 8);
const gpio_t VH(GPIOA, 9);
const gpio_t WH(GPIOA, 10);
const gpio_t UL(GPIOA, 7);
const gpio_t VL(GPIOB, 0);
const gpio_t WL(GPIOB, 1);

// Tim1 Inputs
const gpio_t CPOUT(GPIOA, 12);
const gpio_t BKIN(GPIOA, 6);

// BEMF
const gpio_t BEMF1(GPIOC, 3);
const gpio_t BEMF2(GPIOC, 4);
const gpio_t BEMF3(GPIOC, 5);
const gpio_t GPIO_BEMF(GPIOC, 9);

// Feedback inputs
const gpio_t CURRA(GPIOA, 0);
const gpio_t CURRB(GPIOC, 1);
const gpio_t CURRC(GPIOC, 0);
const gpio_t VBUS(GPIOA, 1);
const gpio_t TEMP(GPIOC, 2);

// DAC Outputs
const gpio_t DAC_1(GPIOA, 4);
const gpio_t DAC_2(GPIOA, 5);

//HALL Inputs
const gpio_t HALL1(GPIOA, 15);
const gpio_t HALL2(GPIOB, 3);
const gpio_t HALL3(GPIOB, 10);

/**
  * @brief  GPIO ports initialization
  * @retval None
  */
void GPIO_Init(void){
	/*
	 * Ports Clock Enable
	 */
	SYSCFGClockEnable();
	GPIO_AllPortsClockEnable();

	/*
	 * User LEDs
	 * PB2 = LEDX Red
	 */
	GPIO_OutputPinInit(LedX, PushPull, High, NoPull);
	GPIO_OutputPinInit(DebugPin_1, PushPull, High, NoPull);

	/*
	 * User Button = PC13
	 */
	GPIO_InputPinInit(Button, NoPull);

}
