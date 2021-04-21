/*
 * gpio.cpp
 *
 *  Created on: 16 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */


#include "main.h"
#include "gpio.h"

/**
  * @brief  GPIO ports initialization
  * @retval None
  */
void GPIO_Init(void){

	/*
	 * GPIOB and GPOIC Ports Clock Enable
	 */
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
	GPIO_PortClockInit(LEDX_PORT);
	GPIO_PortClockInit(BUTTON_PORT);

	/*
	 * User LEDs
	 * PB2 = LEDX Red
	 */
	GPIO_OutputPinInit(LEDX_PIN, LEDX_PORT, PushPull, High, NoPull);

	/*
	 * User Button = PC13
	 */
	GPIO_InputPinInit(BUTTON_PIN, BUTTON_PORT, NoPull);


}
