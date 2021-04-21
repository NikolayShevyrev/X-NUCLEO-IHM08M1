/*
 * dma.h
 *
 *  Created on: 2 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "main.h"

enum PriorityLevels{
	Low 	 = 0b00,
	Medium	 = 0b01,
	High 	 = 0b10,
	VeryHigh = 0b11
};

enum Size{
	Byte 	 = 0b00,
	HalfWord = 0b01,
	Word 	 = 0b10
};

enum DataDiractoin{
	PeripheralToMemory	= 0b00,
	MemoryToPeripheral	= 0b01,
	MemoryToMemory		= 0b10
};

class DMA {
private:
	DMA_TypeDef * dma_base;
	DMA_Channel_TypeDef * ch_base;
	uint16_t channel;
	uint32_t * memoryAddress;
public:
	DMA(DMA_TypeDef * base, uint16_t channel)
		: dma_base(base), channel(channel), memoryAddress(nullptr){
		if(dma_base == DMA1){
			switch(channel){
				case 1:
					ch_base = DMA1_Channel1;
					break;
				case 2:
					ch_base = DMA1_Channel2;
					break;
				case 3:
					ch_base = DMA1_Channel3;
					break;
				case 4:
					ch_base = DMA1_Channel4;
					break;
				case 5:
					ch_base = DMA1_Channel5;
					break;
				case 6:
					ch_base = DMA1_Channel6;
					break;
				case 7:
					ch_base = DMA1_Channel7;
					break;
			}
		} else if(dma_base == DMA2){
			switch(channel){
				case 1:
					ch_base = DMA2_Channel1;
					break;
				case 2:
					ch_base = DMA2_Channel2;
					break;
				case 3:
					ch_base = DMA2_Channel3;
					break;
				case 4:
					ch_base = DMA2_Channel4;
					break;
				case 5:
					ch_base = DMA2_Channel5;
					break;
			}
		}
	}


	void Init(uint32_t * address);

	void PriorityLevel(PriorityLevels level){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			SET_BIT(ch_base->CCR, level << DMA_CCR_PL_Pos);
		}
	}

	void MemoryDataSize(Size size){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			SET_BIT(ch_base->CCR, size << DMA_CCR_MSIZE_Pos);
		}
	}

	void PeripheralDataSize(Size size){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			SET_BIT(ch_base->CCR, size << DMA_CCR_PSIZE_Pos);
		}
	}

	void MemoryIncrementMode(){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			SET_BIT(ch_base->CCR, DMA_CCR_MINC);
		}
	}

	void PeripheralIncrementMode(){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			SET_BIT(ch_base->CCR, DMA_CCR_PINC);
		}
	}

	void CircularMode(){
		SET_BIT(ch_base->CCR, DMA_CCR_CIRC);
	}

	void DataTransferDiraction(DataDiraction dir){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			if(dir == PeripheralToMemory){
				CLEAR_BIT(ch_base->CCR, DMA_CCR_DIR);
			}
			if(dir == MemoryToPeripheral){
				SET_BIT(ch_base->CCR, DMA_CCR_DIR);
			}
			if(dir == MemoryToMemory){
				SET_BIT(ch_base->CCR, DMA_CCR_MEM2MEM);
			}
		}
	}

	void TransferCompleteInterruptEnable(){
		SET_BIT(ch_base->CCR, DMA_CCR_TCIE);
	}

	void ChannelEnable(){
		SET_BIT(ch_base->CCR, DMA_CCR_EN);
	}

	void NumberOfData(uint16_t number){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			WRITE_REG(ch_base->CNDTR, number);
		}
	}

	void PeripheralAddress(uint32_t address){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			WRITE_REG(ch_base->CPAR, address);
		}
	}

	void MemoryAddress(uint32_t address){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			WRITE_REG(ch_base->CMAR, address);
		}
	}

	void SetPriorityIRQ(uint32_t priority){
		if(dma_base == DMA1){
			switch(channel){
				case 1:
					NVIC_SetPriority(DMA1_Channel1_IRQn, priority);
					break;
				case 2:
					NVIC_SetPriority(DMA1_Channel2_IRQn, priority);
					break;
				case 3:
					NVIC_SetPriority(DMA1_Channel3_IRQn, priority);
					break;
				case 4:
					NVIC_SetPriority(DMA1_Channel4_IRQn, priority);
					break;
				case 5:
					NVIC_SetPriority(DMA1_Channel5_IRQn, priority);
					break;
				case 6:
					NVIC_SetPriority(DMA1_Channel6_IRQn, priority);
					break;
				case 7:
					NVIC_SetPriority(DMA1_Channel7_IRQn, priority);
					break;
			}
		} else if(dma_base == DMA2){
			switch(channel){
				case 1:
					NVIC_SetPriority(DMA2_Channel1_IRQn, priority);
					break;
				case 2:
					NVIC_SetPriority(DMA2_Channel2_IRQn, priority);
					break;
				case 3:
					NVIC_SetPriority(DMA2_Channel3_IRQn, priority);
					break;
				case 4:
					NVIC_SetPriority(DMA2_Channel4_IRQn, priority);
					break;
				case 5:
					NVIC_SetPriority(DMA2_Channel5_IRQn, priority);
					break;
			}
		}
	}

	void EnableIRQ(){
		if(dma_base == DMA1){
			switch(channel){
				case 1:
					NVIC_EnableIRQ(DMA1_Channel1_IRQn);
					break;
				case 2:
					NVIC_EnableIRQ(DMA1_Channel2_IRQn);
					break;
				case 3:
					NVIC_EnableIRQ(DMA1_Channel3_IRQn);
					break;
				case 4:
					NVIC_EnableIRQ(DMA1_Channel4_IRQn);
					break;
				case 5:
					NVIC_EnableIRQ(DMA1_Channel5_IRQn);
					break;
				case 6:
					NVIC_EnableIRQ(DMA1_Channel6_IRQn);
					break;
				case 7:
					NVIC_EnableIRQ(DMA1_Channel7_IRQn);
					break;
			}
		} else if(dma_base == DMA2){
			switch(channel){
				case 1:
					NVIC_EnableIRQ(DMA2_Channel1_IRQn);
					break;
				case 2:
					NVIC_EnableIRQ(DMA2_Channel2_IRQn);
					break;
				case 3:
					NVIC_EnableIRQ(DMA2_Channel3_IRQn);
					break;
				case 4:
					NVIC_EnableIRQ(DMA2_Channel4_IRQn);
					break;
				case 5:
					NVIC_EnableIRQ(DMA2_Channel5_IRQn);
					break;
			}
		}
	}

	void DisableIRQ(){
		if(dma_base == DMA1){
			switch(channel){
				case 1:
					NVIC_DisableIRQ(DMA1_Channel1_IRQn);
					break;
				case 2:
					NVIC_DisableIRQ(DMA1_Channel2_IRQn);
					break;
				case 3:
					NVIC_DisableIRQ(DMA1_Channel3_IRQn);
					break;
				case 4:
					NVIC_DisableIRQ(DMA1_Channel4_IRQn);
					break;
				case 5:
					NVIC_DisableIRQ(DMA1_Channel5_IRQn);
					break;
				case 6:
					NVIC_DisableIRQ(DMA1_Channel6_IRQn);
					break;
				case 7:
					NVIC_DisableIRQ(DMA1_Channel7_IRQn);
					break;
			}
		} else if(dma_base == DMA2){
			switch(channel){
				case 1:
					NVIC_DisableIRQ(DMA2_Channel1_IRQn);
					break;
				case 2:
					NVIC_DisableIRQ(DMA2_Channel2_IRQn);
					break;
				case 3:
					NVIC_DisableIRQ(DMA2_Channel3_IRQn);
					break;
				case 4:
					NVIC_DisableIRQ(DMA2_Channel4_IRQn);
					break;
				case 5:
					NVIC_DisableIRQ(DMA2_Channel5_IRQn);
					break;
			}
		}
	}


};


#endif /* INC_DMA_H_ */
