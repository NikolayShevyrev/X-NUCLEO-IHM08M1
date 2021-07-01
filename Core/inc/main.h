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

#define LEDX_ON() SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << 2));
#define LEDX_OFF() SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << (2+16)));
#define LEDX_TOGGLE() GPIOB->ODR ^= (GPIO_ODR_0 << 2);

#define DEBUGPIN_1_ON() SET_BIT(GPIOC->BSRR, (GPIO_BSRR_BS_0  << 8));
#define DEBUGPIN_1_OFF() SET_BIT(GPIOC->BSRR, (GPIO_BSRR_BS_0  << (8+16)));
#define DEBUGPIN_1_TOGGLE() GPIOC->ODR ^= (GPIO_ODR_0 << 8);

/* Defines -------------------------------------------------------------------*/
#define FPWM (uint16_t)20000
#define CONVERSIONS_COUNT 2

#define BEMF_MEAS_PERIOD 300

#define CURRENT_CONV_COEF 	(float)(0.00734F / (float)CONVERSIONS_COUNT)
#define VOLTAGE_CONV_COEF 	(float)(0.0154F / (float)CONVERSIONS_COUNT)

#define BEMF_CONV_COEF 		(float)(0.0146627566F)
#define NTC_CONV_COEF 		(float)(0.00080586F / (float)CONVERSIONS_COUNT)

#define MAX_TEMP 			(uint16_t)100

//#define AWD1_HT				(uint16_t)(2.73F / 0.00080586F) // 100 degres
#define AWD1_HT				(uint16_t)(58.F / 0.0154F) // 100 degres
#define AWD1_LT				0

enum state {
	Stopped,
	Starting,
	Running,
	Stopping,
	Fault
};

#endif /* INC_MAIN_H_ */
