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
#include "gpio.h"


/*
 * Stopped State Handlers
 */
void StoppedState::Timer1InterruptHandler() {

}

void StoppedState::DMA1InterruptHandler() {

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
