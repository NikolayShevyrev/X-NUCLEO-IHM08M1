/*
 * timers.h
 *
 *  Created on: 23 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_TIMERS_H_
#define INC_TIMERS_H_

#include "main.h"


class Timer {
protected:
	TIM_TypeDef * timer_;
public:
	Timer(TIM_TypeDef * timer) :
		  timer_(timer){
	}

	virtual ~Timer(){

	}

	virtual void Init(){

	}

	void Start() const { SET_BIT(timer_->CR1, TIM_CR1_CEN); }

	void Stop() const { CLEAR_BIT(timer_->CR1, TIM_CR1_CEN); }

	void ResetCNT() const { CLEAR_REG(timer_->CNT); }

	virtual void SetCNT(uint16_t cnt) const { WRITE_REG(timer_->CNT, cnt); }

	uint16_t GetCNT() const { return READ_REG(timer_->CNT); }

	virtual void SetARR(uint16_t arr) const { WRITE_REG(timer_->ARR, arr); }

	uint16_t GetARR() const { return READ_REG(timer_->ARR); }

	virtual void SetPSC(uint16_t psc) const { WRITE_REG(timer_->PSC, psc); }

	uint16_t GetPSC() const { return READ_REG(timer_->PSC); }

	void ClearUIF()  const  { CLEAR_BIT(timer_->SR, TIM_SR_UIF); }

	void ClearSR()  const  { CLEAR_REG(timer_->SR); }

};

class Timer1 : public Timer {
private:
	uint16_t period_ = 0;
	uint16_t pwmFrequnce_ = 0;
	uint16_t pwm100usFactor = 0;
	/*
	 * Capture/Compare output enable depending on the commutation sector
	 * 0x1 -> OCx is output, OCxN is not active
	 * 0xC -> OCx is not active, OCxN is output with polarity
	 * 0xD -> OCx is output, OCxN is output with polarity
	 */
	const uint16_t PWM_CCER_STATES_CLKW[6] = {0x0CCD,0x0CCD,0x0CDC,0x0CDC,0x0DCC,0x0DCC};
	uint16_t PWM_CCER_STATES[6];

	/*
	 * Capture/Compare output mode depending on the commutation sector
	 * 0x78 = PWM mode 2
	 * 0x58 = Force active level - OC1REF is forced high
	 * 0x48 = Force inactive level - OC1REF is forced low
	 */
	const uint16_t PWM_CCMR1_STATES_CLKW[6] = {0x5878,0x4878,0x7848,0x7858,0x4858,0x5848};
	const uint16_t PWM_CCMR2_STATES_CLKW[6] = {0x0048,0x0058,0x0058,0x0048,0x0078,0x0078};
	uint16_t PWM_CCMR1_STATES[6];
	uint16_t PWM_CCMR2_STATES[6];
	/*
	 * Master mode selection: ADC Trigger
	 * 0x00000060 = OC3REF signal is used as trigger output (TRGO)
	 * 0x00000050 = OC2REF signal is used as trigger output (TRGO)
	 * 0x00000040 = OC1REF signal is used as trigger output (TRGO)
	 */
	const uint32_t MMS_STATES_CLWS[6] = {0x00000040, 0x00000040, 0x00000050, 0x00000050, 0x00000060, 0x00000060};
	uint32_t MMS_STATES[6];

	const struct {
		uint16_t CCER 	= 0x0CCC;
		uint16_t CCMR1	= 0x4848;
		uint16_t CCMR2	= 0x0048;
	} PWM_STOP_STATE;

public:
	Timer1() : Timer(TIM1) {
	}

	virtual void Init(uint16_t pwmFrequnce);

	void PinsInit();

	void SetCCR1(uint16_t ccr) const { WRITE_REG(timer_->CCR1, ccr); }

	void SetCCR2(uint16_t ccr) const { WRITE_REG(timer_->CCR2, ccr); }

	void SetCCR3(uint16_t ccr) const { WRITE_REG(timer_->CCR3, ccr); }

	void SetCCR4(uint16_t ccr) const { WRITE_REG(timer_->CCR4, ccr); }

	uint16_t GetCCR1() const { return READ_REG(timer_->CCR1); }

	uint16_t GetCCR2() const { return READ_REG(timer_->CCR2); }

	uint16_t GetCCR3() const { return READ_REG(timer_->CCR3); }

	uint16_t GetCCR4() const { return READ_REG(timer_->CCR4); }

	uint16_t GetPWMPeriod(){
		return period_;
	}

	void PWMOutputsOn(){
		SET_BIT(TIM1->BDTR, TIM_BDTR_MOE);
	}

	void PWMOutputsOff(){
		CLEAR_BIT(TIM1->BDTR, TIM_BDTR_MOE);
	}

	void PWMStopState(){

		__disable_irq();

		WRITE_REG(TIM1->CCER, PWM_STOP_STATE.CCER);
		WRITE_REG(TIM1->CCMR1, PWM_STOP_STATE.CCMR1);
		WRITE_REG(TIM1->CCMR2, PWM_STOP_STATE.CCMR2);

		/* Generate Capture/Compare control update event */
		SET_BIT(TIM1->EGR, TIM_EGR_COMG);
		CLEAR_BIT(TIM1->SR, TIM_SR_COMIF);

		__enable_irq();
	}

	uint16_t Getpwm100usFactor(){
		return pwm100usFactor;
	}

	void SetDuty(uint16_t duty){
		if(duty < period_){
			duty = period_ - duty;
		} else {
			duty = 0;
		}
		WRITE_REG(TIM1->CCR1, duty);
		WRITE_REG(TIM1->CCR2, duty);
		WRITE_REG(TIM1->CCR3, duty);
	}

	void SetDiraction(bool dir){
		if(dir) {
			for(int i = 0; i < 6; i++){
				PWM_CCER_STATES[i] = PWM_CCER_STATES_CLKW[i];
				PWM_CCMR1_STATES[i] = PWM_CCMR1_STATES_CLKW[i];
				PWM_CCMR2_STATES[i]	= PWM_CCMR2_STATES_CLKW[i];
				MMS_STATES[i] = MMS_STATES_CLWS[i];
			}
		} else {
			for(int i = 0; i < 6; i++){
				PWM_CCER_STATES[i] = PWM_CCER_STATES_CLKW[5-i];
				PWM_CCMR1_STATES[i] = PWM_CCMR1_STATES_CLKW[5-i];
				PWM_CCMR2_STATES[i]	= PWM_CCMR2_STATES_CLKW[5-i];
				MMS_STATES[i] = MMS_STATES_CLWS[5-i];
			}
		}
	}

	void SwitchCommSector(uint16_t sector){
		WRITE_REG(TIM1->CCER, PWM_CCER_STATES[sector]);
		WRITE_REG(TIM1->CCMR1, PWM_CCMR1_STATES[sector]);
		WRITE_REG(TIM1->CCMR2, PWM_CCMR2_STATES[sector]);
		CLEAR_BIT(TIM1->CR2, TIM_CR2_MMS);
		SET_BIT(TIM1->CR2, MMS_STATES[sector]);

		/* Generate Capture/Compare control update event */
		SET_BIT(TIM1->EGR, TIM_EGR_COMG);
		CLEAR_BIT(TIM1->SR, TIM_SR_COMIF);
	}

};

class Timer2 : public Timer {
public:
	Timer2() : Timer(TIM2) {
	}


	virtual void Init();

	void InitHall();

	void PinsInit();

	virtual void SetCNT(uint32_t cnt) const { WRITE_REG(timer_->CNT, cnt); }

	virtual void SetARR(uint32_t arr) const { WRITE_REG(timer_->ARR, arr); }

	virtual void SetPSC(uint32_t psc) const { WRITE_REG(timer_->PSC, psc); }

	uint32_t GetCNT_32() const { return READ_REG(timer_->CNT); }

	uint32_t GetARR_32() const { return READ_REG(timer_->ARR); }

	uint32_t GetPSC_32() const { return READ_REG(timer_->PSC); }

	void SetCCR1(uint32_t ccr) const { WRITE_REG(timer_->CCR1, ccr); }

	void SetCCR2(uint32_t ccr) const { WRITE_REG(timer_->CCR2, ccr); }

	void SetCCR3(uint32_t ccr) const { WRITE_REG(timer_->CCR3, ccr); }

	void SetCCR4(uint32_t ccr) const { WRITE_REG(timer_->CCR4, ccr); }

	uint32_t GetCCR1() const { return READ_REG(timer_->CCR1); }

	uint32_t GetCCR2() const { return READ_REG(timer_->CCR2); }

	uint32_t GetCCR3() const { return READ_REG(timer_->CCR3); }

	uint32_t GetCCR4() const { return READ_REG(timer_->CCR4); }
};


class Timer6 : public Timer {
public:
	Timer6() : Timer(TIM6) {
	}


	virtual void Init();
};

class Timer4 : public Timer {
public:
	Timer4() : Timer(TIM4) {
	}


	virtual void Init();
};

class Timer3 : public Timer {
public:
	Timer3() : Timer(TIM3) {
		}


	virtual void Init();
};


#endif /* INC_TIMERS_H_ */
