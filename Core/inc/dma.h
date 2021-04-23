/*
 * dma.h
 *
 *  Created on: 2 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include "main.h"
#include <array>
#include "dfilter.h"

enum PriorityLevels {
	LowPr 	 	= 0b00,
	MediumPr 	= 0b01,
	HighPr 	 	= 0b10,
	VeryHighPr 	= 0b11
};

enum Size {
	Byte 	 = 0b00,
	HalfWord = 0b01,
	Word 	 = 0b10
};

enum DataDiraction {
	PeripheralToMemory	= 0b00,
	MemoryToPeripheral	= 0b01,
	MemoryToMemory		= 0b10
};

class DMA {
protected:
	DMA_TypeDef * dma_base;
	DMA_Channel_TypeDef * ch_base;
	uint16_t channel;
public:
	DMA(DMA_TypeDef * base, uint16_t channel)
		: dma_base(base), channel(channel){
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


	virtual void Init(){

	}

	void ClockEnable(){
		if(dma_base == DMA1){
			SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA1EN);
		} else if(dma_base == DMA2){
			SET_BIT(RCC->AHBENR, RCC_AHBENR_DMA2EN);
		}
	}

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

	void TransferCompleteInterruptFlagClear(){
		SET_BIT(dma_base->IFCR, (0x1UL << (DMA_IFCR_CTCIF1_Pos+4*(channel-1))));
	}

	void ChannelEnable(){
		SET_BIT(ch_base->CCR, DMA_CCR_EN);
	}

	void NumberOfData(uint16_t number){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			WRITE_REG(ch_base->CNDTR, number);
		}
	}

	void PeripheralAddress(const uint32_t & address){
		if(!READ_BIT(ch_base->CCR, DMA_CCR_EN)){
			WRITE_REG(ch_base->CPAR, address);
		}
	}

	void MemoryAddress(const uint32_t & address){
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

class DMA1Channel1 : public DMA {
private:
	std::array<uint32_t, (CONVERSIONS_COUNT*3)> data;
public:
	DMA1Channel1() : DMA(DMA1, 1){
	}

	virtual void Init();

	float GetDCCurrent(void){
		float current = 0;
		for(int i = 0 ; i < (CONVERSIONS_COUNT*3); i+=3){
			current += data[i];
		}
		return (current * CURRENT_CONV_COEF);
	}

	float GetDCVoltage(void){
		static dFilter<float, 16> voltageFilter;
		float voltage = 0;
		for(int i = 0 ; i < (CONVERSIONS_COUNT*3); i+=3){
			voltage += data[i];
		}
		voltage = voltage * VOLTAGE_CONV_COEF;
		return voltageFilter.Calc(voltage);
	}

	float GetTemp(void){
		static dFilter<float, 32> tempFilter;
		float temp = 0;
		for(int i = 2 ; i < (CONVERSIONS_COUNT*3); i+=3){
			temp += data[i];
		}
		temp = temp * TEMP_CONV_COEF;
		return tempFilter.Calc(temp);
	}
};

class DMA2Channel1 : public DMA {
private:
	std::array<uint32_t, 3> data;
public:
	DMA2Channel1() : DMA(DMA2, 1){
	}

	virtual void Init();

	float GetBEMF1(void){
		return (data[0] * BEMF_CONV_COEF);
	}

	float GetBEMF2(void){
		return (data[1] * BEMF_CONV_COEF);
	}

	float GetBEMF3(void){
		return (data[2] * BEMF_CONV_COEF);
	}

};


#endif /* INC_DMA_H_ */
