/*
 * mctrl.cpp
 *
 *  Created on: 22 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#include "sixstepcomm.h"

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

	timer_to_rpm = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)rpmTimer->GetPSC_32()));
	timer_min 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)rpmTimer->GetPSC_32() * (float)maxRPM));
	timer_max 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)rpmTimer->GetPSC_32() * (float)StartUp.rpm));

	maxDuty = (uint16_t)((float)pwmTimer->GetPWMPeriod() * (float)0.1);
	minDuty = pwmTimer->GetPWMPeriod();

	stallLimit 		= 14000;
	blankingLimit 	= 4;
}

void SixStepCommutation::Start(){

	startUpDelay.Tick();

	if(StartUp.fRampOn == false){
		Align();
	} else {
		//Ramp();
	}
}

void SixStepCommutation::Run(){

	if(Flags.preCommutation == true) { return; }

	if((++blankingCount) < blankingLimit) { return; }

	if(BEMFDetection() == true){

		stallCount = 0;
		Flags.bemfDetection = true;
		Flags.preCommutation = true;

		rpmTimer->Stop();
		timer_avg = rpmFilter.Calc(rpmTimer->GetCNT());
		rpmTimer->ResetCNT();
		rpmTimer->Start();
		currentRPM = timer_to_rpm/timer_avg;

		commutationTime = timer_avg << 1;
		if(commutationTime < 1){
			commutationTime = 1;
		}
		commTimer->SetARR(commutationTime);
		commTimer->Start();

		SpeedLoopController();

	} else {
		stallCount++;
	}
}

void SixStepCommutation::Stop(){

	pwmTimer->PWMOutputsOff();
	startUpDelay.Stop();
	rpmTimer->Stop();
	rpmFilter.Reset();
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

	pwmTimer->SwitchCommSector(commSector);

	CalcSector();

	startUpDelay.Start(StartUp.Time.sector * pwmTimer->Getpwm100usFactor());
}


void SixStepCommutation::CalcSector(){

	if(StartUp.Time.current < (uint32_t)(StartUp.Time.ramp*10)){

		StartUp.Time.sector = StartUp.sector_constant/StartUp.Time.current;
		if(StartUp.Time.sector <= 0) StartUp.Time.sector = 1;
		StartUp.state = RampOn;

	} else if(StartUp.Time.current < (uint32_t)(StartUp.Time.ramp*10 + StartUp.Time.sust*10)){
		StartUp.state = SustOn;
	} else {
		StartUp.state = StartUpOver;
	}

	StartUp.Time.current += StartUp.Time.sector;
	if(StartUp.Time.current > (uint32_t)4000000000) StartUp.Time.current = (uint32_t)4000000000;
}

bool SixStepCommutation::BEMFDetection(){
	if((comparatorOutputs ^ bemfFilter.XOR[commSector]) & bemfFilter.mask[commSector]){
		bemfFilter.value |= 0x01;
	}
	bemfFilter.value = bemfFilter.filter[bemfFilter.value];
	return (bemfFilter.value & 0x01);
}

void SixStepCommutation::SpeedLoopController(){

	if(Flags.trainPI == true) { return; }

	if(currentRPM < desiredRPM){
		if(currentDuty > minDuty) currentDuty--;
	} else if(currentRPM > desiredRPM){
		if(currentDuty < maxDuty) currentDuty++;
	}

	pwmTimer->SetDuty(currentDuty);
}

