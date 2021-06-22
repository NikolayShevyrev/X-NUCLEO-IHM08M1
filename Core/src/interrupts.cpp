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
#include "TM1637.h"

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

	timer1.ClearUIF();

	timer3.ResetCNT();

	motor.Run(currentState);
}

/**
  * @brief  Timer6  Commutation Interrupt Service Routine
  * @retval None
  */
void TIM6_DAC_IRQHandler(void) {

	extern SixStepCommutation motor;
	//extern state currentState;

	DEBUGPIN_1_OFF();

	motor.Commutation();

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
	motor.SetTemperature();

	dma1ch1.TransferCompleteInterruptFlagClear();

}

/**
  * @brief  DMA2 Channel 1 Interrupt Service Routine
  * @retval None
  */
void DMA2_Channel1_IRQHandler(void) {
	extern DMA2Channel1 dma2ch1;
	extern SixStepCommutation motor;
	extern state currentState;

	//WRITE_REG(DAC->DHR12R2, (motor.adc_data_bemf[2]));

	motor.BemfDetection(currentState);

	dma2ch1.TransferCompleteInterruptFlagClear();


}

/**
  * @brief  ADC1 Interrupt Service Routine
  * @retval None
  */
void ADC1_2_IRQHandler(void) {
	extern ADC_1 adc1;
	extern tm1637 display;
	extern state currentState;

	if(adc1.AWD1GetInterruptFlag() == true){
		adc1.AWD1ClearInterruptFlag();
		currentState = Fault;
		display.display(0x00, '-');
		display.display(0x01, 'O');
		display.display(0x02, 'V');
		display.display(0x03, 'T');
	}
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

