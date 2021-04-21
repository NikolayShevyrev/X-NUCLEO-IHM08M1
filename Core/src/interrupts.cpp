/*
 * interrupts.cpp
 *
 *  Created on: 26 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#include "sixstepcomm.h"
#include "timers.h"
#include "main.h"
#include "delay.h"
#include "gpio.h"
#include "dfilter.h"
#include "motordrive.h"

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/**
  * @brief  Timer1 PWM Interrupt Service Routine
  * @retval None
  */
void TIM1_UP_TIM16_IRQHandler(void) {
	extern Timer1 timer1;
	extern MotorDrive drive;

	drive.Timer1InterruptHandler();

	timer1.ClearUIF();

}

void TIM6_DAC_IRQHandler(void) {
	/*extern Timer6 timer6;

	timer6.ClearUIF();*/
}



#ifdef __cplusplus
}
#endif /* __cplusplus */

