/*
 * mctrl.cpp
 *
 *  Created on: 22 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#include "sixstepcomm.h"
#include "adc.h"

void SixStepCommutation::Init(const SixStepCommSettings& settings){
	StartUp.duty 			= (uint16_t)((float)pwmTimer->GetPWMPeriod() * settings.startup_duty);
	StartUp.rpm 			= settings.startup_rpm;
	pole_pairs 				= settings.pole_pairs;
	StartUp.Time.start 		= 20;
	StartUp.Time.alignment 	= settings.rotor_align_t;
	StartUp.Time.ramp 		= settings.startup_ramp_t;
	StartUp.Time.sust 		= settings.startup_sust_t;
	StartUp.sector_constant =(uint32_t)((float)1000000 * (float)((float)StartUp.Time.ramp/((float)StartUp.rpm * (float)pole_pairs)));

	minRPM 		= StartUp.rpm;
	maxRPM 		= settings.max_rpm;
	desiredRPM	= settings.desired_rpm;

	timer_to_rpm = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)71.f));
	timer_min 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)71.f * (float)maxRPM));
	timer_max 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)71.f * (float)StartUp.rpm));

	minDuty = (uint16_t)((float)pwmTimer->GetPWMPeriod() * (float)0.1);
	maxDuty = pwmTimer->GetPWMPeriod();

	stallLimit 		= 14000;
	blankingLimit 	= 4;
}

void SixStepCommutation::Run(state& currentState){

	switch(currentState){
		case Stopped:
			break;
		case Starting:
			startUpDelay.Tick();
			if(StartUp.fRampOn == false){
				Align();
			} else {
				Ramp();
			}
			break;
		case Running:
			/*if(Flags.preCommutation == false){
				blankingCount++;
			}*/
			if(stallCount > stallLimit){
				currentState = Fault;
			}
			break;
		case Stopping:
			this->Stop();
			break;
		case Fault:
			this->Stop();
			break;
		default:
			break;
	}
}

void SixStepCommutation::BemfDetection(state& currentState){

	if(Flags.preCommutation == true) { return; }

	if((++blankingCount) < blankingLimit) { return; }

	comparatorOutputs = 0;

	Feedback.bemf = (float)adc_data_bemf[bemfDetection.current[commSector]] * BEMF_CONV_COEF;
	if(Feedback.bemf > Feedback.dcVoltage/2){
		comparatorOutputs += bemfDetection.comp[commSector];
	}

	//WRITE_REG(DAC->DHR12R2, (uint16_t)(Feedback.bemf*100));
	WRITE_REG(DAC->DHR12R2, adc_data_bemf[bemfDetection.current[commSector]]);
	WRITE_REG(DAC->DHR12R1, (uint16_t)(Feedback.dcVoltage*100));

	if(BEMFDetection() == true){

		LEDX_ON();

		bemfDetection.flag = true;

		if(bemfDetection.count > 2 && currentState == Starting){
			currentState = Running;
		}

		stallCount = 0;
		Flags.bemfDetection = true;
		Flags.preCommutation = true;

		rpmTimer->Stop();
		timer_avg = rpmFilter.Calc(rpmTimer->GetCNT());
		rpmTimer->ResetCNT();
		rpmTimer->Start();
		if(timer_avg == 0) { timer_avg = 1; }
		currentRPM = timer_to_rpm/timer_avg;


		commutationTime = timer_avg >> 1;
		if(commutationTime < 1){
			commutationTime = 1;
		}
		commTimer->SetARR(commutationTime);

		//commTimer->Start();

		if(currentState == Running){
			DEBUGPIN_1_ON();
			commTimer->Start();
			SpeedLoopController();
		}


	} else {

		LEDX_OFF()

		stallCount++;
	}
}

void SixStepCommutation::Commutation(){
	bemfFilter.value = 0;
	blankingCount = 0;

	//Change commutation sector
	if((++commSector) > 5) { commSector = 0; }

	pwmTimer->SwitchCommSector(commSector);

	Flags.preCommutation = false;

	commTimer->ClearUIF();
	commTimer->Stop();
	commTimer->ResetCNT();


}

void SixStepCommutation::Stop(){

	extern ADC_2 adc2;

	pwmTimer->PWMOutputsOff();
	startUpDelay.Stop();

	rpmTimer->Stop();
	commTimer->Stop();

	rpmTimer->ResetCNT();
	commTimer->ResetCNT();

	rpmFilter.Reset();

	adc2.StopRegularConv();


	Flags.preCommutation = false;
	bemfFilter.value = 0;
	blankingCount = 0;
	stallCount = 0;

}

void SixStepCommutation::Align(){

	if(startUpDelay.GetState() == On) { return; }

	pwmTimer->PWMOutputsOff();

	rpmTimer->Stop();
	rpmTimer->ResetCNT();

	commTimer->Stop();
	commTimer->ResetCNT();

	stallCount = 0;
	blankingCount = 0;

	pwmTimer->SetDiraction(Flags.diraction);

	// Initialize TMR7 and rpm Filter average with the value corresponding to the minimum motor speed
	rpmTimer->SetCNT(timer_max);
	rpmFilter.Reset();
	rpmFilter.FillBuffer(timer_max);

	currentDuty = StartUp.duty;
	pwmTimer->SetDuty(currentDuty);

	commSector = 5;
	pwmTimer->SwitchCommSector(commSector);

	StartUp.Time.current = StartUp.Time.start * 10;
	StartUp.Time.sector = 0;
	StartUp.state = AlignmentOn;
	StartUp.fRampOn = true;

	// Non-blocking delay for alingment
	startUpDelay.Start(StartUp.Time.alignment * pwmTimer->Getpwm100usFactor() * 10);

	pwmTimer->PWMOutputsOn();

}

void SixStepCommutation::Ramp(){

	if(startUpDelay.GetState() == On) { return; }

	//Change commutation sector
	if((++commSector) > 5) { commSector = 0; }

	if (StartUp.state != StartUpOver){
		CalcSector();
	} else {
		Flags.preCommutation = false;
		blankingCount = 0;
		bemfFilter.value = 0;
		if(bemfDetection.flag == true){
			bemfDetection.count++;
			bemfDetection.flag = false;
		} else {
			bemfDetection.count = 0;
			bemfDetection.error++;
		}
		if (bemfDetection.error > bemfDetection.limit){
			bemfDetection.error = 0;
			StartUp.Time.sector = (uint32_t)((float)StartUp.Time.sector * (float)ADVENCE_ANGLE_COEFF);
			if(StartUp.Time.sector < MIN_SECTOR_TIME) { StartUp.Time.sector = MIN_SECTOR_TIME; }
		}

	}

	pwmTimer->SwitchCommSector(commSector);


	startUpDelay.Start(StartUp.Time.sector * pwmTimer->Getpwm100usFactor());
}


void SixStepCommutation::CalcSector(){

	extern ADC_2 adc2;

	if(StartUp.Time.current < (uint32_t)(StartUp.Time.ramp*10)){

		StartUp.Time.sector = StartUp.sector_constant/StartUp.Time.current;
		if(StartUp.Time.sector <= 0) StartUp.Time.sector = 1;
		StartUp.state = RampOn;

	} else if(StartUp.Time.current < (uint32_t)(StartUp.Time.ramp*10 + StartUp.Time.sust*10)){

		StartUp.state = SustOn;

	} else {

		StartUp.state = StartUpOver;
		adc2.StartRegularConv();
		Flags.preCommutation = false;

	}

	StartUp.Time.current += StartUp.Time.sector;
	if(StartUp.Time.current > TIME_LIMIT) StartUp.Time.current = TIME_LIMIT;
}

bool SixStepCommutation::BEMFDetection(){
	if((comparatorOutputs ^ bemfFilter.XOR[commSector]) & bemfFilter.mask[commSector]){
		bemfFilter.value |= 0x01;
	}
	bemfFilter.value = bemfFilter.filter[bemfFilter.value];
	return (bemfFilter.value & 0x01);
}

void SixStepCommutation::SpeedLoopController(){

	if(currentRPM < desiredRPM){
		if(currentDuty < maxDuty) currentDuty++;
	} else if(currentRPM > desiredRPM){
		if(currentDuty > minDuty) currentDuty--;
	}

	pwmTimer->SetDuty(currentDuty);
}

