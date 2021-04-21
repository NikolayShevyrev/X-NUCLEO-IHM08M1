/*
 * adc.cpp
 *
 *  Created on: 31 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#include "adc.h"

void ADC_1::Init(){

	PinsInit();

	ClockEnable(DivBy1);
	VoltageRegulatorEnable();
	Calibration(SingleEnded);

	ContiuousConvertionMode();
	DMAEnable();
	DMAConfiguration(Circular);
	DelayedConversionMode();

	RegularSequenceLength(1);
	ConvertionInRegularSequence(1, 11);

	Enable();
	StartRegularConv();

}

void ADC_1::PinsInit(){
	/*
	 * Ports Clock Enable
	 */
	GPIO_PortClockInit(CURRB);

	/*
	 * PC1 -> Current B Channel 11
	 */
	GPIO_AnalogPinInit(CURRB);

}

void ADC_2::Init(){

	PinsInit();

	ClockEnable(DivBy1);
	VoltageRegulatorEnable();
	Calibration(SingleEnded);

	ContiuousConvertionMode();
	DMAEnable();
	DMAConfiguration(Circular);
	DelayedConversionMode();

	RegularSequenceLength(1);
	ConvertionInRegularSequence(1, 13);

	Enable();
	StartRegularConv();

}

void ADC_2::PinsInit(){
	/*
	 * Ports Clock Enable
	 */
	GPIO_PortClockInit(BEMF1);
	GPIO_PortClockInit(BEMF2);
	GPIO_PortClockInit(BEMF3);

	/*
	 * PC3 -> BEMF1 Channel 13
	 * PC4 -> BEMF2 Channel 14
	 * PC5 -> BEMF3 Channel 15
	 */
	GPIO_AnalogPinInit(BEMF1);
	GPIO_AnalogPinInit(BEMF2);
	GPIO_AnalogPinInit(BEMF3);

}

void ADC_3::Init(){

	PinsInit();

	ClockEnable(DivBy1);
	VoltageRegulatorEnable();
	Calibration(SingleEnded);

	ContiuousConvertionMode();
	DMAEnable();
	DMAConfiguration(Circular);
	DelayedConversionMode();

	RegularSequenceLength(1);
	ConvertionInRegularSequence(1, 1);

	Enable();
	StartRegularConv();

}

void ADC_3::PinsInit(){
	/*
	 * Ports Clock Enable
	 */
	GPIO_PortClockInit(VBUS);
	GPIO_PortClockInit(TEMP);

	/*
	 * PA1 -> VBUS 			Channel 1
	 * PC2 -> Temperature	Channel ?
	 */
	GPIO_AnalogPinInit(VBUS);
	GPIO_AnalogPinInit(TEMP);

}
