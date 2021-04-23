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
#include "dma.h"
#include "adc.h"
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

	GPIO_SetPin(LedX);

	drive.Timer1InterruptHandler();

	GPIO_ResetPin(LedX);

	timer1.ClearUIF();



}

/**
  * @brief  DMA1 Channel 1 Interrupt Service Routine
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void) {
	extern DMA1Channel1 dma1ch1;
	extern MotorDrive drive;

	drive.DMA1InterruptHandler();

	dma1ch1.TransferCompleteInterruptFlagClear();
}



#ifdef __cplusplus
}
#endif /* __cplusplus */

