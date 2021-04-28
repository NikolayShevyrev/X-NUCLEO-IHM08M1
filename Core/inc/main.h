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
#define CONVERSIONS_COUNT 2

#define CURRENT_CONV_COEF 	(float)(0.00734F / (float)CONVERSIONS_COUNT)
#define VOLTAGE_CONV_COEF 	(float)(0.0154F / (float)CONVERSIONS_COUNT)
#define BEMF_CONV_COEF 		(float)(1.F / (float)CONVERSIONS_COUNT)
#define TEMP_CONV_COEF 		(float)(1.F / (float)CONVERSIONS_COUNT)

enum state {
	Stopped,
	Starting,
	Running,
	Stopping,
	Fault
};

#endif /* INC_MAIN_H_ */
