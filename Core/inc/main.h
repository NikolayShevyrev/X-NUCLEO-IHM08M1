/*
 * main.h
 *
 *  Created on: Apr 6, 2021
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_MAIN_H_
#define INC_MAIN_H_

#include "stm32f3xx.h"
#include "stm32f303xe.h"

/* Function prototypes -------------------------------------------------------*/
void SystemClock_Config(void);

/* Defines -------------------------------------------------------------------*/
#define FPWM (uint16_t)20000
#define CONVERSIONS_COUNT 6

#define CURRENT_CONV_COEF 	(float)(0.001F / (float)CONVERSIONS_COUNT)
#define VOLTAGE_CONV_COEF 	(float)(0.01F / (float)CONVERSIONS_COUNT)
#define BEMF_CONV_COEF 		(float)(0.01F / (float)CONVERSIONS_COUNT)

// Leds
#define LEDX_PIN	2
#define LEDX_PORT	GPIOB

// Button
#define BUTTON_PIN 13
#define BUTTON_PORT GPIOC

// PWM Outputs
#define UH_PIN	8
#define UH_PORT	GPIOA
#define UL_PIN	15
#define UL_PORT	GPIOB
#define VH_PIN	9
#define VH_PORT	GPIOA
#define VL_PIN	0
#define VL_PORT	GPIOB
#define WH_PIN	10
#define WH_PORT	GPIOA
#define WL_PIN	1
#define WL_PORT	GPIOB

// Tim1 Inputs
#define CPOUT_PIN	12
#define CPOUT_PORT	GPIOA
#define BKIN_PIN	6
#define BKIN_PORT	GPIOA

// BEMF
#define BEMF1_PIN	3
#define BEMF1_PORT	GPIOC
#define BEMF2_PIN	4
#define BEMF2_PORT	GPIOC
#define BEMF3_PIN	5
#define BEMF3_PORT	GPIOC
#define GPIO_BEMF_PIN	9
#define GPIO_BEMF_PORT	GPIOC

// Feedback inputs
#define CURR_A_PIN	0
#define CURR_A_PORT GPIOA
#define CURR_B_PIN  1
#define CURR_B_PORT GPIOC
#define CURR_C_PIN  0
#define CURR_C_PORT GPIOC
#define VBUS_PIN  	1
#define VBUS_PORT 	GPIOA
#define TEMP_PIN	2
#define TEMP_PORT	GPIOC

// DAC Outputs
#define DAC1_PIN 4
#define DAC2_PIN 5
#define DAC_PORT GPIOA


//HALL Inputs
#define HALL1_IN_PIN 15
#define HALL2_IN_PIN 3
#define HALL3_IN_PIN 10
#define HALL1_IN_PORT GPIOA
#define HALL23_IN_PORT GPIOB

#endif /* INC_MAIN_H_ */
