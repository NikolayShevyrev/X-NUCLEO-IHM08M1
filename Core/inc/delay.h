/*
 * delay.h
 *
 *  Created on: 16 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DELAY_H_
#define INC_DELAY_H_

/* Functions prototypes ---------------------------------------------*/
void DelayUS(uint32_t us);
void DelayMS(uint32_t ms);

enum DelayState{
	Off,
	On
};

class NonBlockingDelay {
private:
	uint32_t limit;
	uint32_t counter;
	DelayState state;
public:
	NonBlockingDelay() :
		limit(0), counter(0), state(Off) {
	}

	bool Start(uint32_t input){
		if(state == On){
			return false;
		}

		limit = input;
		counter = 0;
		state = On;
		return true;
	}

	void Stop(){
		limit = 0;
		counter = 0;
		state = Off;
	}

	void Tick(){
		if(state == Off){
			return;
		}

		if((++counter) >= limit){
			state = Off;
			limit = 0;
			counter = 0;
		}
	}

	DelayState GetState(){
		return state;
	}

};

#endif /* INC_DELAY_H_ */
