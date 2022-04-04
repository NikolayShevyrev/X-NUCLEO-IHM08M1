/*
 * mctrl.cpp
 *
 *  Created on: 22 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#include "sixstepcomm.h"
#include "adc.h"


void SixStepCommutation::Init(const SixStepCommSettings& settings)
{
	StartUp.duty 			= (uint16_t)((float)pwmTimer->GetPWMPeriod() * settings.startup_duty);
	StartUp.initialRPM		= settings.initialRPM;
	StartUp.acceleration	= settings.accelaration;
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

	timer_to_rpm = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)72.f));
	timer_min 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)72.f * (float)maxRPM));
	timer_max 	 = (uint32_t)((float)10 * (float)SystemCoreClock/((float)pole_pairs * (float)72.f * (float)StartUp.rpm));

	minDuty = (uint16_t)((float)pwmTimer->GetPWMPeriod() * (float)0.1f);
	maxDuty = pwmTimer->GetPWMPeriod();

	stallLimit 		= 14000;
	blankingLimit 	= 4;

	//RPM Ramp
	rpmRamp.minRpm 		= settings.rampMinRpm;
	rpmRamp.maxRpm 		= settings.rampMaxRpm;
	rpmRamp.constRpm 	= settings.rampConstRpm;
	rpmRamp.time 		= settings.rampTime;
	rpmRamp.step		= (settings.fpwm * rpmRamp.time) / (rpmRamp.maxRpm - rpmRamp.minRpm);

}

void SixStepCommutation::Run(state& currentState)
{
	extern tm1637 display;

	switch(currentState)
	{
		case Stopped:
			break;
		case Starting:
			startUpDelay.Tick();
			displayDelay.Tick();
			if(StartUp.state == StartUpOff)
			{
				Align();
			}
			else
			{
				Ramp();
			}
			break;
		case Running:
			displayDelay.Tick();
			rpmRampDelay.Tick();
			speedLoopDelay.Tick();
			RPMRamp();
			if(stallCount > stallLimit)
			{
				currentState = Fault;
				display.display(0x00, 'S');
				display.display(0x01, 'S');
				display.display(0x02, 'T');
				display.display(0x03, 'L');
			}
			break;
		case Stopping:
			if(Flags.stopping == false)
			{
				this->Stop();
				display.display(0x00, 'S');
				display.display(0x01, 'T');
				display.display(0x02, 'O');
				display.display(0x03, 'P');
			}
			else
			{
				stopDelay.Tick();
			}
			if(stopDelay.GetState() == Off)
			{
				Flags.stopping = false;
				currentState = Stopped;
				display.display(0x00, 'r');
				//display.display(0x01, 'r');
				//display.display(0x02, 'D');
				//display.display(0x03, 'Y');
			}
			break;
		case Fault:
			if(Flags.stopping == false)
			{
				this->Stop();
			}
			else
			{
				stopDelay.Tick();
			}
			if(stopDelay.GetState() == Off)
			{
				Flags.stopping = false;
				currentState = Stopped;
				display.display(0x00, '-');
				display.display(0x01, 'r');
				display.display(0x02, 'D');
				display.display(0x03, 'Y');
				#ifdef AUTO_RESTART
					currentState = Starting;
				#endif
			}
			break;
		default:
			break;
	}
}

void SixStepCommutation::BemfDetection(state& currentState){

	extern tm1637 display;

	if(Flags.preCommutation == true) { return; }

	if((++blankingCount) < blankingLimit) { return; }

	comparatorOutputs = 0;

	Feedback.bemf = (float)adc_data_bemf[bemfDetection.current[commSector]] * BEMF_CONV_COEF;
	if(Feedback.bemf > Feedback.dcVoltage/2){
		comparatorOutputs += bemfDetection.comp[commSector];
	}

	//WRITE_REG(DAC->DHR12R2, adc_data_bemf[bemfDetection.current[commSector]]);

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

		if(currentState == Running){
			commTimer->Start();
			SpeedLoopController();
		}

		if(displayDelay.GetState() == Off){
			display.displayNum(currentRPM, 0);
			displayDelay.Start(20000);
		}

	} else {

		LEDX_OFF()

		stallCount++;
	}
}

void SixStepCommutation::Commutation(){
	extern state currentState;

	if(currentState == Running || currentState == Starting){

		bemfFilter.value = 0;
		blankingCount = 0;

		//Change commutation sector
		if((++commSector) > 5) { commSector = 0; }

		pwmTimer->SwitchCommSector(commSector);

		Flags.preCommutation = false;

	}

	commTimer->ClearUIF();
	commTimer->Stop();
	commTimer->ResetCNT();


}

void SixStepCommutation::Stop(){

	extern ADC_2 adc2;

	//pwmTimer->PWMOutputsOff();
	adc2.StopRegularConv();
	rpmTimer->Stop();
	commTimer->Stop();
	pwmTimer->PWMStopState();

	startUpDelay.Stop();

	rpmTimer->ResetCNT();
	commTimer->ResetCNT();

	rpmFilter.Reset();

	Flags.preCommutation = false;
	StartUp.state = StartUpOff;
	bemfFilter.value = 0;
	blankingCount = 0;
	stallCount = 0;

	Flags.stopping = true;
	stopDelay.Start(stopTime);

}

void SixStepCommutation::Align(){

	if(startUpDelay.GetState() == On) { return; }

	//pwmTimer->PWMOutputsOff();
	pwmTimer->PWMStopState();

	rpmTimer->Stop();
	rpmTimer->ResetCNT();

	commTimer->Stop();
	commTimer->ResetCNT();

	stallCount = 0;
	blankingCount = 0;
	StartUp.sustCount = 0;

	//Check Switches
	#ifndef SWITCH_STARTUP
		if(GPIO_ReadPin(Switch_1)){
			Flags.diraction = Clockwise;
		} else {
			Flags.diraction = Anticlockwise;
		}
	#else
		Flags.diraction = Clockwise;
	#endif
		SetDiraction(Flags.diraction);

	if(GPIO_ReadPin(Switch_2)){
		rpmRamp.mode = Const;
		desiredRPM = rpmRamp.constRpm;
	} else {
		rpmRamp.mode = Alt;
		rpmRamp.dir = Up;
		desiredRPM = rpmRamp.minRpm;
	}


	// Initialize TMR7 and rpm Filter average with the value corresponding to the minimum motor speed
	currentRPM = StartUp.initialRPM;
	rpmTimer->SetCNT(timer_max);
	rpmFilter.Reset();
	rpmFilter.FillBuffer(timer_to_rpm/currentRPM);


	currentDuty = StartUp.duty;
	pwmTimer->SetDuty(currentDuty);

	commSector = 5;
	pwmTimer->SwitchCommSector(commSector);

	StartUp.Time.current = StartUp.Time.start * 10;
	StartUp.Time.sector = 0;
	StartUp.state = AlignmentOn;

	// Non-blocking delay for alingment
	startUpDelay.Start(StartUp.Time.alignment * pwmTimer->Getpwm100usFactor() * 10);

	pwmTimer->PWMOutputsOn();

}

void SixStepCommutation::Ramp(){

	extern tm1637 display;
	extern state currentState;

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

		currentRPM = (float)timer_to_rpm/rpmFilter.Calc(StartUp.Time.sector*100);
	}

	pwmTimer->SwitchCommSector(commSector);

	if(displayDelay.GetState() == Off){
		display.displayNum(currentRPM, 0);
		displayDelay.Start(20000);
	}

	startUpDelay.Start(StartUp.Time.sector * pwmTimer->Getpwm100usFactor());


	if (currentRPM >= maxRPM) {
		currentState = Fault;
		if(Flags.stopping == false){
			Stop();
		}
		display.display(0x00, 'S');
		display.display(0x01, 'D');
		display.display(0x02, 'N');
		display.display(0x03, 'S');
	}
}

void SixStepCommutation::CalcSector(){

	extern ADC_2 adc2;

	if(StartUp.state == AlignmentOn){

		StartUp.Time.sector = (timer_to_rpm/currentRPM)/(uint32_t)100;
		if(StartUp.Time.sector <= 0) StartUp.Time.sector = 1;
		StartUp.state = RampOn;

	} else if(StartUp.state == RampOn){

		StartUp.Time.sector = (uint32_t)((float)StartUp.Time.sector*StartUp.acceleration);
		currentRPM = (float)timer_to_rpm/rpmFilter.Calc(StartUp.Time.sector*100);
		if(StartUp.Time.sector <= 0) {
			StartUp.Time.sector = 1;
		}
		if(currentRPM >= StartUp.rpm){
			StartUp.state = SustOn;
		}

	} else if(StartUp.sustCount < StartUp.sustLimit){

		StartUp.sustCount++;
		currentRPM = (float)timer_to_rpm/rpmFilter.Calc(StartUp.Time.sector*100);
		StartUp.state = SustOn;

	} else {

		currentRPM = (float)timer_to_rpm/rpmFilter.Calc(StartUp.Time.sector*100);
		StartUp.state = StartUpOver;
		Flags.preCommutation = false;
		adc2.StartRegularConv();

	}

}

bool SixStepCommutation::BEMFDetection(){
	if((comparatorOutputs ^ bemfFilter.XOR[commSector]) & bemfFilter.mask[commSector]){
		bemfFilter.value |= 0x01;
	}
	bemfFilter.value = bemfFilter.filter[bemfFilter.value];
	return (bemfFilter.value & 0x01);
}

void SixStepCommutation::SpeedLoopController(){

	if(speedLoopDelay.GetState() == On){ return; }

	if(currentRPM < desiredRPM){
		if(currentDuty < maxDuty) currentDuty++;
	} else if(currentRPM > desiredRPM){
		if(currentDuty > minDuty) currentDuty--;
	}

	pwmTimer->SetDuty(currentDuty);

	speedLoopDelay.Start(speedLoopTime);
}

void SixStepCommutation::RPMRamp(){

	if(rpmRamp.mode == Const) { return; }

	if(rpmRampDelay.GetState() == On) { return; }

	if(rpmRamp.dir == Up){
		desiredRPM++;
		if(desiredRPM >= rpmRamp.maxRpm){
			rpmRamp.dir = Down;
		}
		rpmRampDelay.Start(rpmRamp.step);
	} else {
		desiredRPM--;
		if(desiredRPM <= rpmRamp.minRpm){
			rpmRamp.dir = Up;
		}
		rpmRampDelay.Start(rpmRamp.step);
	}

}

void SixStepCommutation::SetDiraction(bool dir){

	pwmTimer->SetDiraction(dir);
	if(dir) {
		for(int i = 0; i < 6; i++){
			 bemfDetection.current[i] = bemfDetection.current_clkw[i];
		}
	} else {
		for(int i = 0; i < 6; i++){
			bemfDetection.current[i] = bemfDetection.current_clkw[5-i];
		}
	}
}
