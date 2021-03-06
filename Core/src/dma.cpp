/*
 * dma.cpp
 *
 *  Created on: 22 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */


#include "dma.h"
#include "sixstepcomm.h"

/*
 * DMA1 for Current, Voltage and Temperature measurement
 */
void DMA1Channel1::Init(){

	extern SixStepCommutation motor;

	ClockEnable();

	DataTransferDiraction(PeripheralToMemory);
	MemoryIncrementMode();
	CircularMode();


	PeripheralDataSize(Word);
	MemoryDataSize(Word);

	WRITE_REG(DMA1_Channel1->CPAR, 	(uint32_t)&ADC1->DR);
	WRITE_REG(DMA1_Channel1->CMAR, 	(uint32_t)&motor.adc_data[0]);

	NumberOfData(CONVERSIONS_COUNT*3);

	PriorityLevel(HighPr);
	TransferCompleteInterruptEnable();

	SetPriorityIRQ(1);
	EnableIRQ();

	ChannelEnable();
}

void DMA2Channel1::Init(){

	extern SixStepCommutation motor;

	ClockEnable();

	DataTransferDiraction(PeripheralToMemory);
	MemoryIncrementMode();
	CircularMode();

	PeripheralDataSize(Word);
	MemoryDataSize(Word);

	WRITE_REG(DMA2_Channel1->CPAR, 	(uint32_t)&ADC2->DR);
	WRITE_REG(DMA2_Channel1->CMAR, 	(uint32_t)&motor.adc_data_bemf[0]);

	NumberOfData(3);

	PriorityLevel(VeryHighPr);
	TransferCompleteInterruptEnable();

	SetPriorityIRQ(1);
	EnableIRQ();

	ChannelEnable();
}
