/*
 * delay.cpp
 *
 *  Created on: 16 окт. 2020 г.
 *      Author:Nikolay Shevyrev
 */
#include "main.h"
#include "delay.h"

void DelayUS(uint32_t us) {
	uint32_t nCount = ((SystemCoreClock/10000000)*us)/3;
	for(; nCount != 0; nCount--);
}

void DelayMS(uint32_t ms) {
	uint32_t nCount = ((SystemCoreClock/10000)*ms)/3;
	for(; nCount != 0; nCount--);
}
