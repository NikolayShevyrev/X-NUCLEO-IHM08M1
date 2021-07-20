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

enum HardwareTrigger {
	Disabled				= 0b00,
	RisingEdge				= 0b01,
	FallingEdge				= 0b10,
	RisingAndFallingEdges	= 0b11
};


class ADC {
protected:
	ADC_TypeDef * base_;
public:
	ADC(ADC_TypeDef * base) : base_(base){
	}

	virtual ~ADC(){

	}

	virtual void Init(){

	}

	virtual void PinsInit(){

	}

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

	void ExternalTriggerEnableRegularChannels(HardwareTrigger trigger){
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART)){
			SET_BIT(base_->CFGR, trigger << ADC_CFGR_EXTEN_Pos);
		}
	}

	void ExternalTriggerSelectionRegularChannels(uint16_t event){
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART)){
			SET_BIT(base_->CFGR, event << ADC_CFGR_EXTSEL_Pos);
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

	void InjectedSequenceLength(uint16_t length){
		SET_BIT(base_->JSQR, ((length-1) << ADC_JSQR_JL_Pos));
	}

	void ConvertionInInjectedSequence(uint16_t convertion, uint16_t channel){
		SET_BIT(base_->JSQR, (channel << (2U + 6 * convertion)));
	}

	/*
	 * Analog Watchdog 1
	 */

	void AWD1ChannelSelection(uint16_t channel) const {
		SET_BIT(base_->CFGR, channel << ADC_CFGR_AWD1CH_Pos);
	}

	void AWD1EnableOnInjectedChannels() const {
		SET_BIT(base_->CFGR, ADC_CFGR_JAWD1EN);
	}

	void AWD1EnableOnRegularChannels() const {
		SET_BIT(base_->CFGR, ADC_CFGR_AWD1EN);
	}

	void AWD1OnAllChannels() const {
		CLEAR_BIT(base_->CFGR, ADC_CFGR_AWD1SGL);
	}

	void AWD1OnSingleChannel() const {
		SET_BIT(base_->CFGR, ADC_CFGR_AWD1SGL);
	}

	/*
	 * Analog Watchdog 2 Configuration Register
	 */
	void AWD2ChannelSelection(uint16_t channel){
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->AWD2CR, ADC_AWD2CR_AWD2CH_0 << channel);
		}
	}

	/*
	 * Analog Watchdog 3 Configuration Register
	 */
	void AWD3ChannelSelection(uint16_t channel){
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->AWD3CR, ADC_AWD3CR_AWD3CH_0 << channel);
		}
	}

	/*
	 * Analog Watchdog Threshold Registers
	 */
	void AWD1HigherThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR1, ADC_TR1_HT1);
			SET_BIT(base_->TR1, threshold << ADC_TR1_HT1_Pos);
		}
	}

	void AWD1LowerThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR1, ADC_TR1_LT1);
			SET_BIT(base_->TR1, threshold << ADC_TR1_LT1_Pos);
		}
	}

	void AWD2HigherThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR2, ADC_TR2_HT2);
			SET_BIT(base_->TR2, threshold << ADC_TR2_HT2_Pos);
		}
	}

	void AWD2LowerThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR2, ADC_TR2_LT2);
			SET_BIT(base_->TR2, threshold << ADC_TR2_LT2_Pos);
		}
	}

	void AWD3HigherThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR3, ADC_TR3_HT3);
			SET_BIT(base_->TR3, threshold << ADC_TR3_HT3_Pos);
		}
	}

	void AWD3LowerThreshold(uint16_t threshold) const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			CLEAR_BIT(base_->TR3, ADC_TR3_LT3);
			SET_BIT(base_->TR3, threshold << ADC_TR3_LT3_Pos);
		}
	}

	/*
	 * Analog Watchdogs Interrupts
	 */

	void AWD1InterruptEnable() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->IER, ADC_IER_AWD1IE);
		}
	}

	bool AWD1GetInterruptFlag() const {
		return READ_BIT(base_->ISR, ADC_ISR_AWD1);
	}

	void AWD1ClearInterruptFlag() const {
		SET_BIT(base_->ISR, ADC_ISR_AWD1);
	}

	void AWD2InterruptEnable() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->IER, ADC_IER_AWD2IE);
		}
	}

	bool AWD2GetInterruptFlag() const {
		return READ_BIT(base_->ISR, ADC_ISR_AWD2);
	}

	void AWD2ClearInterruptFlag() const {
		CLEAR_BIT(base_->ISR, ADC_ISR_AWD2);
	}

	void AWD3InterruptEnable() const {
		if(!READ_BIT(base_->CR, ADC_CR_ADSTART) && !READ_BIT(base_->CR, ADC_CR_JADSTART)){
			SET_BIT(base_->IER, ADC_IER_AWD3IE);
		}
	}

	bool AWD3GetInterruptFlag() const {
		return READ_BIT(base_->ISR, ADC_ISR_AWD3);
	}

	void AWD3ClearInterruptFlag() const {
		CLEAR_BIT(base_->ISR, ADC_ISR_AWD3);
	}
};

class ADC_1 : public ADC {
public:
	ADC_1() : ADC(ADC1){
	}

	virtual void Init();
	virtual void PinsInit();
	void AWD1Init();
	void AWD2Init();
	void AWD3Init();

};

class ADC_2 : public ADC {
public:
	ADC_2() : ADC(ADC2){
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
