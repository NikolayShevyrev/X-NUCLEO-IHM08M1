/*
 * mctrl.h
 *
 *  Created on: 22 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_SIXSTEPCOMM_H_
#define INC_SIXSTEPCOMM_H_

#include "main.h"
#include "timers.h"
#include "delay.h"
#include "settings.h"
#include "dfilter.h"

enum Diraction {
	Clockwise = true,
	Anticlockwise = false
};
enum StartUpState {
	AlignmentOn,
	RampOn,
	SustOn,
	StartUpOver
};

struct MotorStartUp {
	struct {
		uint32_t alignment;
		uint32_t start;
		uint32_t ramp;
		uint32_t sust;
		uint32_t current;
		uint32_t sector;
	} Time;

	uint16_t duty;
	uint16_t rpm;
	uint32_t sector_constant;

	bool fRampOn = false;
	StartUpState state;

};

struct BEMF_Filter {
	const uint16_t mask[6] 		= {0x0004,0x0002,0x0001,0x0004,0x0002,0x0001};
	const uint16_t XOR [6]		= {0x0000,0xFFFF,0x0000,0xFFFF,0x0000,0xFFFF};
	const uint16_t filter [64] 	= {0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
			 	 	 	 	 	   0x20,0x22,0x24,0x26,0x28,0x2A,0x2C,0x2E,0x01,0x01,0x01,0x36,0x01,0x3A,0x3C,0x3E,
								   0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x01,0x01,0x01,0x16,0x01,0x1A,0x1C,0x1E,
								   0x01,0x01,0x01,0x26,0x01,0x2A,0x2C,0x2E,0x01,0x01,0x01,0x36,0x01,0x3A,0x3C,0x3E};
	uint16_t value;
};

class SixStepCommutation {
public:
	uint32_t adc_data_bemf[3];
	uint32_t adc_data[CONVERSIONS_COUNT*3];
private:
	Timer1 * pwmTimer;
	Timer2 * rpmTimer;
	Timer6 * commTimer;

	NonBlockingDelay startUpDelay;
	dFilter<uint16_t, 4> rpmFilter;

	MotorStartUp StartUp;
	BEMF_Filter bemfFilter;

	struct {
		bool trainPI 	= true;
		bool startUp 	= false;
		bool runMotor	= false;
		bool diraction	= true;
		bool preCommutation = false;
		bool bemfDetection = false;
	} Flags;

	struct {
		float bemf1;
		float bemf2;
		float bemf3;
		float dcCurrent;
		float dcVoltage;
		float temperature;
	} Feedback;

	uint8_t commSector;
	uint16_t comparatorOutputs;

	uint32_t timer_to_rpm;
	uint32_t timer_min;
	uint32_t timer_max;
	uint32_t timer_avg;

	uint32_t commutationTime;

	uint16_t stallCount;
	uint16_t blankingCount;

	uint16_t stallLimit;
	uint16_t blankingLimit;

	uint16_t currentRPM;
	uint16_t desiredRPM;

	uint16_t minRPM;
	uint16_t maxRPM;

	uint16_t currentDuty;
	uint16_t maxDuty;
	uint16_t minDuty;

	uint16_t pole_pairs;

public:
	SixStepCommutation(Timer1 * pwm, Timer2 * rpm, Timer6 * comm) :
					   pwmTimer(pwm), rpmTimer(rpm), commTimer(comm) {

	}

	void Init(const SixStepCommSettings& motorSettings);

	void SetDiraction(Diraction dir){
		Flags.diraction = dir;
	}

	void SetBemf1(float bemf){
		Feedback.bemf1 = bemf;
	}

	void SetBemf2(float bemf){
		Feedback.bemf2 = bemf;
	}

	void SetBemf3(float bemf){
		Feedback.bemf3 = bemf;
	}

	void SetDCCurrent(){
		float current = 0;
		for(int i = 2 ; i < (CONVERSIONS_COUNT*3); i+=3){
			current += adc_data[i];
		}
		Feedback.dcCurrent = (current * CURRENT_CONV_COEF);
	}

	void SetDCVoltage(){
		static dFilter<float, 16> voltageFilter;
		float voltage = 0;
		for(int i = 1 ; i < (CONVERSIONS_COUNT*3); i+=3){
			voltage += adc_data[i];
		}
		voltage = voltage * VOLTAGE_CONV_COEF;
		Feedback.dcVoltage = voltageFilter.Calc(voltage);
	}

	void SetTemperature(float temp){
		Feedback.temperature = temp;
	}

	void Start();

	void Run();

	void Stop();

private:
	void Ramp();

	void Align();

    void SpeedLoopController();

    void CalcSector();

    bool BEMFDetection();

};



#endif /* INC_SIXSTEPCOMM_H_ */
