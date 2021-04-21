/*
 * dfilter.h
 *
 *  Created on: 30 окт. 2020 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_DFILTER_H_
#define INC_DFILTER_H_

#include <array>

template<typename T ,std::size_t buff_size>
class dFilter {
private:
	std::array<T, buff_size> buffer;
	uint32_t index;
	T sum;
	uint32_t buffer_size;
public:
	dFilter()
		: index(0), sum(0), buffer_size(buff_size)
	{
		buffer.fill(0);
	}

	void Reset(){
		index = 0;
		sum = 0;
		buffer.fill(0);
	}

	void FillBuffer(T value){
		buffer.fill(value);
	}

	T Calc(T value){
		sum = sum + (value - buffer[index]);
		buffer[index] = value;
		index = (index + 1) & (buffer_size - 1);
		return (sum / buffer_size);
	}
};


#endif /* INC_DFILTER_H_ */
