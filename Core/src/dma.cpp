/*
 * dma.cpp
 *
 *  Created on: 22 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */


#include "dma.h"

void DMA1Channel1::Init(){

	DataTransferDiraction(PeripheralToMemory);
	MemoryIncrementMode();

	PeripheralDataSize(Word);
	MemoryDataSize(Word);

	PeripheralAddress(const_cast<uint32_t&>(ADC1->DR));
	MemoryAddress(data[0]);

	NumberOfData(CONVERSIONS_COUNT*3);

	PriorityLevel(VeryHighPr);
	TransferCompleteInterruptEnable();

	SetPriorityIRQ(0);
	EnableIRQ();

	ChannelEnable();
}

void DMA2Channel1::Init(){

}
