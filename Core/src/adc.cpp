/*
 * adc.cpp
 *
 *  Created on: 31 мар. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#include "adc.h"

void ADC_Init(){

}

void ADCPins_Init(){
	/*
	 * Ports Clock Enable
	 */
	GPIO_PortClockInit(BEMF1_PORT);
	GPIO_PortClockInit(VBUS_PORT);
	GPIO_PortClockInit(CURR_B_PORT);

	/*
	 * PA0 -> Current A
	 * PC1 -> Current B
	 * PC0 -> Current C
	 * PA1 -> VBUS
	 * PC2 -> Temperature
	 */
	GPIO_AnalogPinInit(CURR_A_PIN, CURR_A_PORT);
	GPIO_AnalogPinInit(CURR_B_PIN, CURR_B_PORT);
	GPIO_AnalogPinInit(CURR_C_PIN, CURR_C_PORT);
	GPIO_AnalogPinInit(VBUS_PIN, VBUS_PORT);
	GPIO_AnalogPinInit(TEMP_PIN, TEMP_PORT);

	/*
	 * PC3 -> BEMF1
	 * PC4 -> BEMF2
	 * PC5 -> BEMF3
	 */
	GPIO_AnalogPinInit(BEMF1_PIN, BEMF1_PORT);
	GPIO_AnalogPinInit(BEMF2_PIN, BEMF2_PORT);
	GPIO_AnalogPinInit(BEMF3_PIN, BEMF3_PORT);

	/*
	 * PC9 -> GPIO_BEMF
	 */
	GPIO_OutputPinInit(GPIO_BEMF_PIN, GPIO_BEMF_PORT, PushPull, High, NoPull);
}
