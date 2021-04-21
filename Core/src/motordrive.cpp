/*
 * motordrive.cpp
 *
 *  Created on: 8 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */


#include "motordrive.h"
#include "sixstepcomm.h"
#include "measurement.h"

/*
 * Stopped State Handlers
 */
void StoppedState::Timer1InterruptHandler() {

}

void StoppedState::DMAInterruptHandler() {

}

/*
 * StartingState Handlers
 */
void StartingState::Timer1InterruptHandler() {

}

void StartingState::DMAInterruptHandler() {

}
