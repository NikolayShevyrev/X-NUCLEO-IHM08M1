/*
 * adc.h
 *
 *  Created on: 31 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

#include "main.h"
#include "delay.h"
#include "gpio.h"

enum ADCClockDiv {
	DivDisabled = 0b00000,
	DivBy1 	 = 0b10000,
	DivBy2 	 = 0b10001,
	DivBy4	 = 0b10010,
	DivBy6 	 = 0b10011,
	DivBy8 	 = 0b10100,
	DivBy10  = 0b10101,
	DivBy12  = 0b10110,
	DivBy16  = 0b10111,
	DivBy32  = 0b11000,
	DivBy64  = 0b11001,
	DivBy128 = 0b11010,
	DivBy256 = 0b11011,
};

enum ADCCalibrationMode {
	SingleEnded,
	Differential
};

enum ADC_DMA_mode {
	OneShot,
	Circular
};

class ADC {
protected:
	ADC_TypeDef * base_;
public:
	ADC(ADC_TypeDef * base) : base_(base){
	}

	virtual void Init();

	virtual void PinsInit();

	void ClockEnable(ADCClockDiv div) const {
		if((base_ == ADC1)|| (base_ == ADC2)){
			uint16_t currentDiv = (READ_BIT(RCC->CFGR2, RCC_CFGR2_ADCPRE12) >> RCC_CFGR2_ADCPRE12_Pos);
			if(currentDiv == DivDisabled){
				SET_BIT(RCC->CFGR2, div << RCC_CFGR2_ADCPRE12_Pos);
				SET_BIT(RCC->AHBENR, 		RCC_AHBENR_ADC12EN);
			}
		} else if((base_ == ADC3) || (base_ == ADC4)) {
			uint16_t currentDiv = (READ_BIT(RCC->CFGR2, RCC_CFGR2_ADCPRE34) >> RCC_CFGR2_ADCPRE34_Pos);
			if(currentDiv == DivDisabled){
				SET_BIT(RCC->CFGR2, div << RCC_CFGR2_ADCPRE34_Pos);
				SET_BIT(RCC->AHBENR, 		RCC_AHBENR_ADC34EN);
			}
		}
	}

	void VoltageRegulatorEnable() const {
		CLEAR_BIT(base_->CR, ADC_CR_ADVREGEN); 		// 00: Intermediate state
		SET_BIT(base_->CR,   ADC_CR_ADVREGEN_0); 	// 01: ADC Voltage regulator enabled.
		DelayUS(10);								// Wait for the startup time of the ADC voltage regulator
	}

	void Calibration(ADCCalibrationMode mode) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADEN) && !READ_BIT(base_->CR, ADC_CR_ADCAL)){
			if(mode == SingleEnded){
				CLEAR_BIT(base_->CR, ADC_CR_ADCALDIF); 		// Single-ended inputs Mode
			} else if (mode == Differential){
				SET_BIT(base_->CR, 	ADC_CR_ADCALDIF); 		// Differential inputs Mode
			}
			SET_BIT(base_->CR,  			ADC_CR_ADCAL); 		// ADC calibration
			while(READ_BIT(base_->CR ,	ADC_CR_ADCAL)); 	// Wait for end of ADC calibration
		}
	}

	void Enable() const {
		SET_BIT(base_->CR, ADC_CR_ADEN);
		while(!READ_BIT(base_->ISR, ADC_ISR_ADRDY));
	}

	void Disable() const {
		if(READ_BIT(base_->CR, ADC_CR_ADEN) && !READ_BIT(base_->CR, ADC_CR_ADSTART)
											&& !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->CR, ADC_CR_ADDIS);
		}
	}

	void StartRegularConv() const {
		if(READ_BIT(base_->CR, ADC_CR_ADEN) && !READ_BIT(base_->CR, ADC_CR_ADDIS)){
			SET_BIT(base_->CR, ADC_CR_ADSTART);
		}
	}

	void StartInjectedConv() const {
		if(READ_BIT(base_->CR, ADC_CR_ADEN) && !READ_BIT(base_->CR, ADC_CR_ADDIS)
											&& !READ_BIT(base_->CFGR, ADC_CFGR_JAUTO)){
			SET_BIT(base_->CR, ADC_CR_JADSTART);
		}
	}

	void StopRegularConv() const {
		if(READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_ADDIS)){
			SET_BIT(base_->CR, ADC_CR_ADSTP);
		}
	}

	void StopInjectedConv() const {
		if(READ_BIT(base_->CR, ADC_CR_JADSTART) && !READ_BIT(base_->CR, ADC_CR_ADDIS)){
			SET_BIT(base_->CR, ADC_CR_JADSTP);
		}
	}

	void DMAEnable() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->CFGR, ADC_CFGR_DMAEN);
		}
	}

	void DMADisable() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->CFGR, ADC_CFGR_DMAEN);
		}
	}

	void DMAConfiguration(ADC_DMA_mode mode) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			if(mode == OneShot){
				CLEAR_BIT(base_->CFGR, ADC_CFGR_DMACFG);
			} else if (mode == Circular){
				SET_BIT(base_->CFGR, ADC_CFGR_DMACFG);
			}
		}
	}

	void ContiuousConvertionMode() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CFGR, ADC_CFGR_DISCEN)){
			SET_BIT(base_->CFGR, ADC_CFGR_CONT);
		}
	}

	void SingleConvertionMode() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART)){
			CLEAR_BIT(base_->CFGR, ADC_CFGR_CONT);
		}
	}

	void DelayedConversionMode() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->CFGR, ADC_CFGR_AUTDLY);
		}
	}

	void RegularSequenceLength(uint16_t length) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART)){
			SET_BIT(base_->SQR1, (length-1));
		}
	}

	void ConvertionInRegularSequence(uint16_t convertion, uint16_t channel) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART)){
			if(convertion > 0 && convertion < 5){
				SET_BIT(base_->SQR1, (channel << (ADC_SQR1_SQ1_Pos * convertion)));
			} else if(convertion > 4 && convertion < 10){
				SET_BIT(base_->SQR2, (channel << (ADC_SQR2_SQ6_Pos * (convertion-5))));
			} else if(convertion > 9 && convertion < 15){
				SET_BIT(base_->SQR3, (channel << (ADC_SQR3_SQ11_Pos * (convertion-10))));
			} else if(convertion > 14 && convertion < 17){
				SET_BIT(base_->SQR4, (channel << (ADC_SQR4_SQ16_Pos * (convertion-15))));
			}
		}
	}


};

class ADC_1 : public ADC {
public:
	ADC_1() : ADC(ADC1){
	}

	virtual void Init();
	virtual void PinsInit();
};

class ADC_2 : public ADC {
public:
	ADC_2() : ADC(ADC2){
	}

	virtual void Init();
	virtual void PinsInit();
};

class ADC_3 : public ADC {
public:
	ADC_3() : ADC(ADC3){
	}

	virtual void Init();
	virtual void PinsInit();
};


/*static inline void BEMFPullHigh(){
	GPIO_SetPin(GPIO_BEMF);
}

static inline void BEMFPullLow(){
	GPIO_ResetPin(GPIO_BEMF);
}

static inline void ADCSwitchBEMFChannel(uint16_t sector){

}*/


#endif /* INC_ADC_H_ */
