/*
 * adc.cpp
 *
 *  Created on: 31 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#include "adc.h"

/*
 * ADC1 for Current, Voltage and Temperature measurement
 */
void ADC_1::Init(){

	PinsInit();

	ClockEnable(DivBy1);
	VoltageRegulatorEnable();
	Calibration(SingleEnded);

	SingleConvertionMode();
	DMAEnable();
	DMAConfiguration(Circular);
	DelayedConversionMode();

	RegularSequenceLength(CONVERSIONS_COUNT*3);
	ConvertionInRegularSequence(1, 8);
	ConvertionInRegularSequence(2, 2);
	ConvertionInRegularSequence(3, 7);
	ConvertionInRegularSequence(4, 8);
	ConvertionInRegularSequence(5, 2);
	ConvertionInRegularSequence(6, 7);

	Enable();

}

void ADC_1::PinsInit(){
	/*
	 * PC1 -> Current B 	Channel 7
	 * PA1 -> VBUS 			Channel 2
	 * PC2 -> Temperature	Channel 8
	 */
	GPIO_AnalogPinInit(CURRB);
	GPIO_AnalogPinInit(VBUS);
	GPIO_AnalogPinInit(TEMP);
}

/*
 *  ADC2 for BEFM measurement
 */
void ADC_2::Init(){

	PinsInit();

	ClockEnable(DivBy1);
	VoltageRegulatorEnable();
	Calibration(SingleEnded);

	SingleConvertionMode();
	DMAEnable();
	DMAConfiguration(Circular);
	DelayedConversionMode();

	RegularSequenceLength(3);
	ConvertionInRegularSequence(1, 9);
	ConvertionInRegularSequence(2, 5);
	ConvertionInRegularSequence(3, 11);

	Enable();
}

void ADC_2::PinsInit(){
	/*
	 * PC3 -> BEMF1 Channel 9
	 * PC4 -> BEMF2 Channel 5
	 * PC5 -> BEMF3 Channel 11
	 */
	GPIO_AnalogPinInit(BEMF1);
	GPIO_AnalogPinInit(BEMF2);
	GPIO_AnalogPinInit(BEMF3);
}
