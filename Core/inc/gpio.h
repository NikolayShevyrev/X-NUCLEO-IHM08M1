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
#include <utility>

class gpio_t{
public:
	GPIO_TypeDef* port;
	uint32_t pin;

	gpio_t(GPIO_TypeDef* port, uint32_t pin) : port(port), pin(pin){

	}
};

/* Variables ---------------------------------------------------------*/
// LEDs
extern const gpio_t LedX;

// Buttons
extern const gpio_t Button;

// PWM Outputs
extern const gpio_t UH;
extern const gpio_t VH;
extern const gpio_t WH;
extern const gpio_t UL;
extern const gpio_t VL;
extern const gpio_t WL;

// Tim1 Inputs
extern const gpio_t CPOUT;
extern const gpio_t BKIN;

// BEMF
extern const gpio_t BEMF1;
extern const gpio_t BEMF2;
extern const gpio_t BEMF3;
extern const gpio_t GPIO_BEMF;

// Feedback inputs
extern const gpio_t CURRA;
extern const gpio_t CURRB;
extern const gpio_t CURRC;
extern const gpio_t VBUS;
extern const gpio_t TEMP;

// DAC Outputs
extern const gpio_t DAC_1;
extern const gpio_t DAC_2;

//HALL Inputs
extern const gpio_t HALL1;
extern const gpio_t HALL2;
extern const gpio_t HALL3;


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

static inline void SYSCFGClockEnable(){
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
}
static inline void GPIO_PortClockInit(gpio_t gpio)
{
	switch((uint32_t)gpio.port) {
		case (uint32_t)GPIOA_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOAEN);
			}
			break;
		case (uint32_t)GPIOB_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIOBEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOBEN);
			}
			break;
		case (uint32_t)GPIOC_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIOCEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOCEN);
			}
			break;
		case (uint32_t)GPIOD_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIODEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIODEN);
			}
			break;
		case (uint32_t)GPIOE_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIOEEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOEEN);
			}
			break;
		case (uint32_t)GPIOF_BASE:
			if(!READ_BIT(RCC->AHBENR, RCC_AHBENR_GPIOFEN)){
				SET_BIT(RCC->AHBENR, RCC_AHBENR_GPIOFEN);
			}
			break;
	}
}

static inline void GPIO_OutputPinInit(gpio_t gpio, OutputType type , OutputSpeed speed, IOPull pull)
{
	CLEAR_BIT(gpio.port->MODER, (GPIO_MODER_MODER0 << (gpio.pin*2)));
	SET_BIT(gpio.port->MODER, (GPIO_MODER_MODER0_0 << (gpio.pin*2))); 	// Port mode configuration

	CLEAR_BIT(gpio.port->OTYPER, (GPIO_OTYPER_OT_0 << gpio.pin));
	SET_BIT(gpio.port->OTYPER, (type << gpio.pin));						// Output type configuration

	CLEAR_BIT(gpio.port->OSPEEDR, (GPIO_OSPEEDER_OSPEEDR0 << (gpio.pin*2)));
	SET_BIT(gpio.port->OSPEEDR, (speed << (gpio.pin*2)));					// Output speed configuration

	CLEAR_BIT(gpio.port->PUPDR, (GPIO_PUPDR_PUPDR0 << (gpio.pin*2)));
	SET_BIT(gpio.port->PUPDR, (pull << (gpio.pin*2)));					// Pull configuration

}

static inline void GPIO_InputPinInit(gpio_t gpio, IOPull pull)
{
	CLEAR_BIT(gpio.port->MODER, (GPIO_MODER_MODER0 << (gpio.pin*2))); 	// Port mode configuration

	CLEAR_BIT(gpio.port->PUPDR, (GPIO_PUPDR_PUPDR0 << (gpio.pin*2)));
	SET_BIT(gpio.port->PUPDR, (pull << (gpio.pin*2)));					// Pull configuration

}

static inline void GPIO_AnalogPinInit(gpio_t gpio)
{
	CLEAR_BIT(gpio.port->MODER, (GPIO_MODER_MODER0 << (gpio.pin*2)));
	SET_BIT(gpio.port->MODER, (GPIO_MODER_MODER0 << (gpio.pin*2))); 	// Port mode configuration

	CLEAR_BIT(gpio.port->PUPDR, (GPIO_PUPDR_PUPDR0 << (gpio.pin*2)));		// Pull configuration

}

static inline void GPIO_AFPinInit(gpio_t gpio, OutputType type , OutputSpeed speed, IOPull pull, AlternateFunction af)
{
	CLEAR_BIT(gpio.port->MODER, (GPIO_MODER_MODER0 << (gpio.pin*2)));
	SET_BIT(gpio.port->MODER, (GPIO_MODER_MODER0_1 << (gpio.pin*2))); 	// Port mode configuration

	CLEAR_BIT(gpio.port->OTYPER, (GPIO_OTYPER_OT_0 << gpio.pin));
	SET_BIT(gpio.port->OTYPER, (type << gpio.pin));						// Output type configuration

	CLEAR_BIT(gpio.port->OSPEEDR, (GPIO_OSPEEDER_OSPEEDR0 << (gpio.pin*2)));
	SET_BIT(gpio.port->OSPEEDR, (speed << (gpio.pin*2)));					// Output speed configuration

	CLEAR_BIT(gpio.port->PUPDR, (GPIO_PUPDR_PUPDR0 << (gpio.pin*2)));
	SET_BIT(gpio.port->PUPDR, (pull << (gpio.pin*2)));					// Pull configuration

	if(gpio.pin <= 7) {
		CLEAR_BIT(gpio.port->AFR[0], (0xF << (gpio.pin * 4)));
		SET_BIT(gpio.port->AFR[0], (af << (gpio.pin * 4)));
	} else {
		CLEAR_BIT(gpio.port->AFR[1], (0xF << ((gpio.pin-8) * 4)));
		SET_BIT(gpio.port->AFR[1], (af << ((gpio.pin-8) * 4)));
	}


}

static inline void GPIO_SetPin(gpio_t gpio) {
	SET_BIT(gpio.port->BSRR, (GPIO_BSRR_BS_0  << gpio.pin));
}

static inline void GPIO_ResetPin(gpio_t gpio) {
	SET_BIT(gpio.port->BSRR, (GPIO_BSRR_BS_0  << (gpio.pin+16)));
}

static inline void GPIO_TogglePin(gpio_t gpio) {
	gpio.port->ODR ^= (GPIO_ODR_0 << gpio.pin);
}

static inline uint16_t GPIO_ReadPin(gpio_t gpio) {
	return READ_BIT(gpio.port->IDR, GPIO_IDR_0 << gpio.pin);
}

#endif /* INC_GPIO_H_ */
