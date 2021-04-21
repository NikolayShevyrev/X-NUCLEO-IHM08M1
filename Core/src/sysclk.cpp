/*
 * sysclk.cpp
 *
 *  Created on: 15 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */


#include "main.h"

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void){

	CLEAR_BIT(RCC->CR, RCC_CR_PLLON); 						// PLL Off
	while(READ_BIT(RCC->CR, RCC_CR_PLLRDY));				// Wait till PLL is ready

	/*
	 * HSE Configuration
	  */
	SET_BIT(RCC->CR, RCC_CR_HSEON);							// HSE oscillator ON
	while(!READ_BIT(RCC->CR,RCC_CR_HSERDY)); 				// Wait till HSE is ready

	/*
	 * PLL Configuration
	 */
	SET_BIT(RCC->CFGR, RCC_CFGR_PLLSRC_HSE_PREDIV); 		// HSE is source for PLL
	SET_BIT(RCC->CFGR, RCC_CFGR_PLLMUL9);					// PLL multiplyer x9
	SET_BIT(RCC->CR, RCC_CR_PLLON); 						// PLL On
	while(!READ_BIT(RCC->CR, RCC_CR_PLLRDY));				// Wait till PLL is ready

	/*
	 * Flash latency configuration
	 */
	SET_BIT(FLASH -> ACR, FLASH_ACR_LATENCY_2);

	/*
	 * HCLK configuration
	 */
	CLEAR_BIT(RCC -> CFGR, RCC_CFGR_PPRE1);   				// APB1: HCLK divided by 2
	SET_BIT(RCC -> CFGR, RCC_CFGR_PPRE1_2);

	CLEAR_BIT(RCC -> CFGR, RCC_CFGR_PPRE2); 				// APB2: HCLK not divided

	/*
	 * SYSCLK Configuration
	 */
	CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW);
	SET_BIT(RCC->CFGR, RCC_CFGR_SW_PLL);					// PLL is system bus
	while((READ_BIT(RCC->CFGR, RCC_CFGR_SWS)) != RCC_CFGR_SWS_PLL); // wait for sw ready

	SystemCoreClockUpdate();								// Update SystemCoreClock variable according to Clock Register Values

}
