/*
 * dfilter.h
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DFILTER_H_
#define INC_DFILTER_H_

template<typename T ,uint32_t buff_size>
class dFilter {
private:
	T buffer[buff_size];
	uint32_t index;
	T sum;
	uint32_t buffer_size;
public:
	dFilter()
		: index(0), sum(0), buffer_size(buff_size)
	{
		for(uint32_t i = 0; i < buffer_size; i++){
			buffer[i] = 0;
		}
	}

	void Reset(){
		index = 0;
		sum = 0;
		for(uint32_t i = 0; i < buffer_size; i++){
			buffer[i] = 0;
		}
	}

	void FillBuffer(T value){
		for(uint32_t i = 0; i < buffer_size; i++){
			buffer[i] = value;
		}
		sum = value * buffer_size;
	}

	T Calc(T value){
		sum = sum + (value - buffer[index]);
		buffer[index] = value;
		index = (index + 1) & (buffer_size - 1);
		return (sum / buffer_size);
	}
};


#endif /* INC_DFILTER_H_ */
