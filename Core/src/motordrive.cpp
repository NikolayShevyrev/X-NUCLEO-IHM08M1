/*
 * motordrive.cpp
 *
 *  Created on: 8 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */


#include "motordrive.h"
#include "sixstepcomm.h"
#include "dma.h"
#include "adc.h"


/*
 * Stopped State Handlers
 */
void StoppedState::Timer1InterruptHandler() {
	extern ADC_1 adc1;

	adc1.StartRegularConv();
}

void StoppedState::DMA1InterruptHandler() {
	extern DMA1Channel1 dma1ch1;
	extern SixStepCommutation motor;

	motor.SetDCCurrent(dma1ch1.GetDCCurrent());
	motor.SetDCVoltage(dma1ch1.GetDCVoltage());
	motor.SetTemperature(dma1ch1.GetTemp());

}

void StoppedState::DMA2InterruptHandler() {

}

/*
 * StartingState Handlers
 */
void StartingState::Timer1InterruptHandler() {

}

void StartingState::DMA1InterruptHandler() {

}

void StartingState::DMA2InterruptHandler() {

}
