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

#define TIME_LIMIT (uint32_t)4000000000
#define MIN_SECTOR_TIME (uint32_t)8
#define ADVENCE_ANGLE_COEFF (float)0.9F

enum Diraction {
	Clockwise = true,
	Anticlockwise = false
};
enum StartUpState {
	StartUpOff,
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
	uint16_t initialRPM;
	float acceleration;
	uint16_t rpm;
	uint32_t sector_constant;

	uint32_t sustCount = 0;
	uint32_t sustLimit = 12;

	bool fRampOn = false;
	StartUpState state = StartUpOff;

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

struct BEMF_Detection {
	bool flag = false;
	uint32_t count;
	uint32_t error;
	const uint32_t limit = 10;
	const uint16_t current[6] = {2, 1, 0, 2, 1, 0};
	const uint16_t comp[6] = {4, 2, 1, 4, 2, 1};
};

class SixStepCommutation {
public:
	uint32_t adc_data_bemf[3];
	uint32_t adc_data[CONVERSIONS_COUNT*3];
private:
	Timer1 * pwmTimer;
	Timer4 * rpmTimer;
	Timer6 * commTimer;

	uint32_t tmp;

	NonBlockingDelay startUpDelay;
	NonBlockingDelay displayDelay;
	dFilter<uint32_t, 4> rpmFilter;

	MotorStartUp StartUp;
	BEMF_Filter bemfFilter;
	BEMF_Detection bemfDetection;

	struct {
		bool trainPI 	= true;
		bool startUp 	= false;
		bool runMotor	= false;
		bool diraction	= false;
		bool preCommutation = false;
		bool bemfDetection = false;
	} Flags;

	struct {
		float bemf;
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
	SixStepCommutation(Timer1 * pwm, Timer4 * rpm, Timer6 * comm) :
					   pwmTimer(pwm), rpmTimer(rpm), commTimer(comm) {

	}

	void Init(const SixStepCommSettings& motorSettings);

	void SetDiraction(Diraction dir){
		Flags.diraction = dir;
	}

	/*void SetBemf1(float bemf){
		Feedback.bemf1 = bemf;
	}

	void SetBemf2(float bemf){
		Feedback.bemf2 = bemf;
	}

	void SetBemf3(float bemf){
		Feedback.bemf3 = bemf;
	}*/

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

	void Run(state& currentState);

	void BemfDetection(state& currentState);

	void Commutation();

	void Stop();

private:
	void Ramp();

	void Align();

    void SpeedLoopController();

    void CalcSector();

    bool BEMFDetection();

};



#endif /* INC_SIXSTEPCOMM_H_ */
