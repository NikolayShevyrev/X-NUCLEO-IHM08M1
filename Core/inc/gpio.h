/*
 * gpio2.h
 *
 * For STM32F3xx
 *
 *  Created on: 19 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

#include "main.h"

enum OutputType {
	PushPull,
	OpenDrain
};

enum OutputSpeed {
	Low,
	Medium,
	High,
	VeryHigh
};

enum IOPull {
	NoPull,
	PullUp,
	PullDown
};

enum AlternateFunction {
	AF0,
	AF1,
	AF2,
	AF3,
	AF4,
	AF5,
	AF6,
	AF7,
	AF8,
	AF9,
	AF10,
	AF11,
	AF12,
	AF13,
	AF14,
	AF15,
};

/* Functions prototypes ---------------------------------------------*/
void GPIO_Init(void);

static inline void GPIO_PortClockInit(GPIO_TypeDef* port)
{
	switch((uint32_t)port) {
		case (uint32_t)GPIOA_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN);
			break;
		case (uint32_t)GPIOB_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOBEN);
			break;
		case (uint32_t)GPIOC_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOCEN);
			break;
		case (uint32_t)GPIOD_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIODEN);
			break;
		case (uint32_t)GPIOE_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOEEN);
			break;
		case (uint32_t)GPIOF_BASE:
			SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOFEN);
			break;
	}
}

static inline void GPIO_OutputPinInit(uint16_t pin, GPIO_TypeDef* port,
				  	  	  	  	  	  OutputType type , OutputSpeed speed, IOPull pull)
{
	CLEAR_BIT(port->MODER, (GPIO_MODER_MODER0 << (pin*2)));
	SET_BIT(port->MODER, (GPIO_MODER_MODER0_0 << (pin*2))); 	// Port mode configuration

	CLEAR_BIT(port->OTYPER, (GPIO_OTYPER_OT_0 << pin));
	SET_BIT(port->OTYPER, (type << pin));						// Output type configuration

	CLEAR_BIT(port->OSPEEDR, (GPIO_OSPEEDER_OSPEEDR0 << (pin*2)));
	SET_BIT(port->OSPEEDR, (speed << (pin*2)));					// Output speed configuration

	CLEAR_BIT(port->PUPDR, (GPIO_PUPDR_PUPDR0 << (pin*2)));
	SET_BIT(port->PUPDR, (pull << (pin*2)));					// Pull configuration

}

static inline void GPIO_InputPinInit(uint16_t pin, GPIO_TypeDef* port, IOPull pull)
{
	CLEAR_BIT(port->MODER, (GPIO_MODER_MODER0 << (pin*2))); 	// Port mode configuration

	CLEAR_BIT(port->PUPDR, (GPIO_PUPDR_PUPDR0 << (pin*2)));
	SET_BIT(port->PUPDR, (pull << (pin*2)));					// Pull configuration

}

static inline void GPIO_AnalogPinInit(uint16_t pin, GPIO_TypeDef* port)
{
	CLEAR_BIT(port->MODER, (GPIO_MODER_MODER0 << (pin*2)));
	SET_BIT(port->MODER, (GPIO_MODER_MODER0 << (pin*2))); 	// Port mode configuration

	CLEAR_BIT(port->PUPDR, (GPIO_PUPDR_PUPDR0 << (pin*2)));		// Pull configuration

}

static inline void GPIO_AFPinInit(uint16_t pin, GPIO_TypeDef* port,
								  OutputType type , OutputSpeed speed, IOPull pull, AlternateFunction af)
{
	CLEAR_BIT(port->MODER, (GPIO_MODER_MODER0 << (pin*2)));
	SET_BIT(port->MODER, (GPIO_MODER_MODER0_1 << (pin*2))); 	// Port mode configuration

	CLEAR_BIT(port->OTYPER, (GPIO_OTYPER_OT_0 << pin));
	SET_BIT(port->OTYPER, (type << pin));						// Output type configuration

	CLEAR_BIT(port->OSPEEDR, (GPIO_OSPEEDER_OSPEEDR0 << (pin*2)));
	SET_BIT(port->OSPEEDR, (speed << (pin*2)));					// Output speed configuration

	CLEAR_BIT(port->PUPDR, (GPIO_PUPDR_PUPDR0 << (pin*2)));
	SET_BIT(port->PUPDR, (pull << (pin*2)));					// Pull configuration

	if(pin <= 7) {
		CLEAR_BIT(port->AFR[0], (0xF << (pin * 4)));
		SET_BIT(port->AFR[0], (af << (pin * 4)));
	} else {
		CLEAR_BIT(port->AFR[1], (0xF << ((pin-8) * 4)));
		SET_BIT(port->AFR[1], (af << ((pin-8) * 4)));
	}


}

static inline void GPIO_SetPin(uint16_t pin, GPIO_TypeDef* port) {
	SET_BIT(port->BSRR, (GPIO_BSRR_BS_0  << pin));
}

static inline void GPIO_ResetPin(uint16_t pin, GPIO_TypeDef* port) {
	SET_BIT(port->BSRR, (GPIO_BSRR_BS_0  << (pin+16)));
}

static inline void GPIO_TogglePin(uint16_t pin, GPIO_TypeDef* port) {
	port->ODR ^= (GPIO_ODR_0 << pin);
}

static inline uint16_t GPIO_ReadPin(uint16_t pin, GPIO_TypeDef* port) {
	return READ_BIT(port->IDR, GPIO_IDR_0 << pin);
}

#endif /* INC_GPIO_H_ */
