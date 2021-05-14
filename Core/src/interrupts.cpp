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

#ifdef __cplusplus
 extern "C" {
#endif /* __cplusplus */

/**
  * @brief  Timer1 PWM Interrupt Service Routine
  * @retval None
  */
void TIM1_UP_TIM16_IRQHandler(void) {

	extern Timer1 timer1;
	extern Timer3 timer3;
	extern SixStepCommutation motor;
	extern state currentState;

	SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << 2));
	//timer1.ClearUIF();
	CLEAR_BIT(TIM1->SR, TIM_SR_UIF);

	SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << (2+16)));

	//timer3.ResetCNT();
	CLEAR_REG(TIM3->CNT);

	if(currentState == Starting){
		//SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << 2));
		motor.Start();
		//SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << (2+16)));
	}

}

/**
  * @brief  DMA1 Channel 1 Interrupt Service Routine
  * @retval None
  */
void DMA1_Channel1_IRQHandler(void) {
	extern DMA1Channel1 dma1ch1;
	extern SixStepCommutation motor;

	motor.SetDCCurrent();
	motor.SetDCVoltage();

	//dma1ch1.TransferCompleteInterruptFlagClear();
	SET_BIT(DMA1->IFCR, DMA_IFCR_CTCIF1);
}

/**
  * @brief  DMA2 Channel 1 Interrupt Service Routine
  * @retval None
  */
void DMA2_Channel1_IRQHandler(void) {
	extern DMA2Channel1 dma2ch1;
	extern SixStepCommutation motor;

	//SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << 2));

	//dma2ch1.TransferCompleteInterruptFlagClear();
	SET_BIT(DMA2->IFCR, DMA_IFCR_CTCIF1);

	//SET_BIT(GPIOB->BSRR, (GPIO_BSRR_BS_0  << (2+16)));

}

#ifdef __cplusplus
}
#endif /* __cplusplus */

