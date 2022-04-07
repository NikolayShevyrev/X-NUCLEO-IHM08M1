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
#include "TM1637.h"

#define TIME_LIMIT (uint32_t)4000000000
#define MIN_SECTOR_TIME (uint32_t)8
#define ADVENCE_ANGLE_COEFF (float)0.95F

enum Diraction {
	Clockwise = true,
	Anticlockwise = false
};

enum RPMMode {
	Const,
	Alt
};

enum RpmRampDir {
	Up,
	Down
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
	const uint32_t limit = 6;
	const uint16_t current_clkw[6] = {2, 1, 0, 2, 1, 0};
	uint16_t current[6];
	const uint16_t comp[6] = {4, 2, 1, 4, 2, 1};
};

class SixStepCommutation {
public:
	uint32_t adc_data_bemf[3];
	uint32_t adc_data[CONVERSIONS_COUNT*3];


	bool faultReset	= false;


private:
	Timer1 * pwmTimer;
	Timer4 * rpmTimer;
	Timer6 * commTimer;

	uint32_t tmp;

	const uint32_t ntcResistance[34] = { 188500, 144290, 111330, 86560, 67790, 53460,
										 42450, 33930, 27280, 22070, 17960, 14700, 12090,
										 10000, 8310, 6940, 5830, 4910, 4160, 3540, 3020,
										 2590, 2230, 1920, 1670, 1450, 1270, 1110, 975, 860,
										 760, 674, 599, 534};

	NonBlockingDelay startUpDelay;
	NonBlockingDelay displayDelay;

	dFilter<uint32_t, 4> rpmFilter;

	MotorStartUp StartUp;
	BEMF_Filter bemfFilter;
	BEMF_Detection bemfDetection;

	struct {
		bool trainPI 	= true;
		bool stopping	= false;
		bool startUp 	= false;
		bool runMotor	= false;
		bool diraction	= true;
		bool preCommutation = false;
		bool bemfDetection = false;
	} Flags;

	struct {
		RPMMode mode = Const;
		uint16_t minRpm;
		uint16_t maxRpm;
		uint16_t constRpm;
		uint32_t time;
		uint32_t step;
		RpmRampDir dir = Up;
	} rpmRamp;

	NonBlockingDelay rpmRampDelay;

	NonBlockingDelay stopDelay;
	uint32_t stopTime = 1200000;//100000;//600000; // = t(s) * fpwm

	NonBlockingDelay speedLoopDelay;
	uint32_t speedLoopTime = 10000; // = t(s) * fpwm

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

	uint32_t currentRPM;
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


	void SetTemperature(){
		float u2 = 0, ntc = 0;
		for(int i = 0; i < (CONVERSIONS_COUNT*3); i+=3){
			u2 += adc_data[i];
		}
		u2 = u2 * NTC_CONV_COEF;
		ntc = 4700.f * (3.3f - u2) / u2;

		//Find closest resistance
		uint16_t index = 0;
		for (int i = 1; i < 34; i++){
			if(ntc < ntcResistance[i]){
				index = i;
			} else {
				if((ntc - ntcResistance[i]) < (ntcResistance[index] - ntc)){
					index = i;
				}
				break;
			}
		}
		Feedback.temperature = ((float)index * 5.f) - 40.f;
	}

	void Protection(state& currentState){
		extern tm1637 display;

		if(Feedback.temperature >= MAX_TEMP){
			currentState = Fault;
			if(Flags.stopping == false){
				Stop();
			}
			display.display(0x00, 'S');
			display.display(0x01, 'O');
			display.display(0x02, 'V');
			display.display(0x03, 'T');
		}

		if(Feedback.dcVoltage <= MIN_DCVOLTAGE){
			currentState = Fault;
			if(Flags.stopping == false){
				Stop();
			}
			display.display(0x00, 'S');
			display.display(0x01, 'L');
			display.display(0x02, 'O');
			display.display(0x03, 'V');
		}

		if(Feedback.dcCurrent > MAX_DCCURRENT){
			currentState = Fault;
			if(Flags.stopping == false){
				Stop();
			}
			display.display(0x00, 'S');
			display.display(0x01, 'O');
			display.display(0x02, 'V');
			display.display(0x03, 'C');
		}
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

    void RPMRamp();

    void CalcSector();

    bool BEMFDetection();

    void SetDiraction(bool dir);

};



#endif /* INC_SIXSTEPCOMM_H_ */
