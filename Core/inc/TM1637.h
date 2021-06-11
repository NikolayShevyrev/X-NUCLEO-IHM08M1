/*
 * TM1637.h
 *
 *  Created on: 31 мая 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_TM1637_H_
#define INC_TM1637_H_

#include "main.h"
#include <math.h>

/*******************Definitions for TM1637*********************/
#define ADDR_AUTO 0x40
#define ADDR_FIXED 0x44

#define STARTADDR 0xc0

/*****Definitions for the clock point of the digit tube *******/
#define POINT_ON 1
#define POINT_OFF 0

/**************Definitions for brightness**********************/
#define BRIGHT_DARKEST 0
#define BRIGHT_TYPICAL 2
#define BRIGHT 4
#define BRIGHTEST 7

class tm1637 {
private:
	GPIO_TypeDef* clk_port_;
	uint32_t clk_pin_;
	GPIO_TypeDef* dio_port_;
	uint32_t dio_pin_;

	uint8_t cmd_set_data_;
	uint8_t cmd_set_addr_;
	uint8_t cmd_disp_ctrl_;

	const int8_t tube_tab[16] = { 0x3f, 0x06, 0x5b, 0x4f,
								0x66, 0x6d, 0x7d, 0x07,
								0x7f, 0x6f, 0x77, 0x7c,
								0x39, 0x5e, 0x79, 0x71 }; //0~9,A,b,C,d,E,F

public:
	tm1637(GPIO_TypeDef* clk_port, uint32_t clk_pin, GPIO_TypeDef* dio_port, uint32_t dio_pin) :
	clk_port_(clk_port), clk_pin_(clk_pin), dio_port_(dio_port), dio_pin_(dio_pin){
	}

	void initDisplay(uint8_t brightness){
		cmd_disp_ctrl_ = 0x88 + brightness;
		clearDisplay();
	}

	void clearDisplay(void) {
	    display(0x00, 0x7f);
	    display(0x01, 0x7f);
	    display(0x02, 0x7f);
	    display(0x03, 0x7f);
	}

	void displayNum(float num, int decimal) {
	    // Displays number with decimal places (no decimal point implementation)
	    // Colon is used instead of decimal point if decimal == 2
	    // Be aware of int size limitations (up to +-2^15 = +-32767)

	    int number = round(fabs(num) * pow(10, decimal));

	    for (int i = 0; i < 4; ++i) {
	        int j = 4 - i - 1;

	        if (number != 0) {
	            display(j, number % 10);
	        } else {
	            display(j, 0x7f);    // display nothing
	        }

	        number /= 10;
	    }

	}

	void display(uint8_t bit_addr, int8_t disp_data){
		int8_t seg_data;

		seg_data = coding(disp_data);

		start();               // Start signal sent to TM1637 from MCU
		writeByte(ADDR_FIXED); // Command1: Set data
		stop();
		start();
		writeByte(bit_addr | 0xc0); // Command2: Set data (fixed address)
		writeByte(seg_data);        // Transfer display data 8 bits
		stop();
		start();
		writeByte(cmd_disp_ctrl_); // Control display
		stop();
	}

private:
	void Delay_us(uint16_t us){
		for( ; us > 0; us--);
		__NOP();
	}

	void clk_high(){
		SET_BIT(clk_port_->BSRR, (GPIO_BSRR_BS_0  << clk_pin_));
	}

	void clk_low(){
		SET_BIT(clk_port_->BSRR, (GPIO_BSRR_BS_0  << (clk_pin_+16)));
	}

	void dio_high(){
		SET_BIT(dio_port_->BSRR, (GPIO_BSRR_BS_0  << dio_pin_));
	}

	void dio_low(){
		SET_BIT(dio_port_->BSRR, (GPIO_BSRR_BS_0  << (dio_pin_+16)));
	}

	void dio_input_mode(){
		CLEAR_BIT(dio_port_->MODER, GPIO_MODER_MODER0 << (dio_pin_ * 2));
	}

	void dio_output_mode(){
		SET_BIT(dio_port_->MODER, GPIO_MODER_MODER0_0  << (dio_pin_ * 2));
	}

	void start(){
		clk_high();
		dio_high();
		Delay_us(2);
		dio_low();
	}

	void stop(){
		clk_low();
		Delay_us(2);
		dio_low();
		Delay_us(2);
		clk_high();
		Delay_us(2);
		dio_high();
	}

	void ask(){
		clk_low();
		Delay_us(5);
		dio_input_mode();
		while(READ_BIT(dio_port_->IDR, GPIO_IDR_0 << dio_pin_));
		clk_high();
		Delay_us(2);
		clk_low();
		dio_output_mode();
	}

	uint8_t char2segments(char c) {
		switch (c) {
			case '_' : return 0x08;
			case '^' : return 0x01; // ¯
			case '-' : return 0x40;
			case '*' : return 0x63; // °
			case ' ' : return 0x00; // space
			case 'A' : return 0x77; // upper case A
			case 'a' : return 0x5f; // lower case a
			case 'B' :              // lower case b
			case 'b' : return 0x7c; // lower case b
			case 'C' : return 0x39; // upper case C
			case 'c' : return 0x58; // lower case c
			case 'D' :              // lower case d
			case 'd' : return 0x5e; // lower case d
			case 'E' :              // upper case E
			case 'e' : return 0x79; // upper case E
			case 'F' :              // upper case F
			case 'f' : return 0x71; // upper case F
			case 'G' :              // upper case G
			case 'g' : return 0x35; // upper case G
			case 'H' : return 0x76; // upper case H
			case 'h' : return 0x74; // lower case h
			case 'I' : return 0x06; // 1
			case 'i' : return 0x04; // lower case i
			case 'J' : return 0x1e; // upper case J
			case 'j' : return 0x16; // lower case j
			case 'K' :              // upper case K
			case 'k' : return 0x75; // upper case K
			case 'L' :              // upper case L
			case 'l' : return 0x38; // upper case L
			case 'M' :              // twice tall n
			case 'm' : return 0x37; // twice tall ∩
			case 'N' :              // lower case n
			case 'n' : return 0x54; // lower case n
			case 'O' :              // lower case o
			case 'o' : return 0x5c; // lower case o
			case 'P' :              // upper case P
			case 'p' : return 0x73; // upper case P
			case 'Q' : return 0x7b; // upper case Q
			case 'q' : return 0x67; // lower case q
			case 'R' :              // lower case r
			case 'r' : return 0x50; // lower case r
			case 'S' :              // 5
			case 's' : return 0x6d; // 5
			case 'T' :              // lower case t
			case 't' : return 0x78; // lower case t
			case 'U' :              // lower case u
			case 'u' : return 0x1c; // lower case u
			case 'V' :              // twice tall u
			case 'v' : return 0x3e; // twice tall u
			case 'W' : return 0x7e; // upside down A
			case 'w' : return 0x2a; // separated w
			case 'X' :              // upper case H
			case 'x' : return 0x76; // upper case H
			case 'Y' :              // lower case y
			case 'y' : return 0x6e; // lower case y
			case 'Z' :              // separated Z
			case 'z' : return 0x1b; // separated Z
		}
		return 0;
	}

	int8_t coding(int8_t disp_data){

		if (disp_data == 0x7f) {
			disp_data = 0x00;    // Clear digit
		} else  if (disp_data >= '0' && disp_data <= '9') {
			disp_data = tube_tab[disp_data - 48];    // char to int (char "0" = ASCII 48)
		} else if (disp_data >= 0 && disp_data < 16) {
			disp_data = tube_tab[disp_data];
		} else {
			disp_data = char2segments(disp_data);
		}

		return disp_data;
	}

	void writeByte (unsigned char oneByte) {
		unsigned char i;
		for (i = 0; i < 8; i++) {
			clk_low();
			if (oneByte & 0x01){
				dio_high();
			}else{
				dio_low();
			}
			Delay_us(3);
			oneByte = oneByte >> 1;
			clk_high();
			Delay_us(3);
		}
		clk_low();
		dio_high();
		clk_high();
		dio_input_mode();
		if(!READ_BIT(dio_port_->IDR, GPIO_IDR_0 << dio_pin_)){
			dio_output_mode();
			dio_low();
		}
		Delay_us(50);
		dio_output_mode();
		Delay_us(50);
	}

};


#endif /* INC_TM1637_H_ */
