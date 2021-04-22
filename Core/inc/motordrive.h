/*
 * motordrive.h
 *
 *  Created on: 8 апр. 2021 г.
 *      Author: Nikolay Shevyrev
 */

#ifndef INC_MOTORDRIVE_H_
#define INC_MOTORDRIVE_H_

#include "main.h"

class MotorDrive;

class State {
protected:
	MotorDrive * motorDrive_;
public:
	State(){
	}

	virtual ~State(){
	}

	void setMotorDrive(MotorDrive * motorDrive){
		this->motorDrive_ = motorDrive;
	}

	virtual void Timer1InterruptHandler() = 0;
	virtual void DMA1InterruptHandler() = 0;
	virtual void DMA2InterruptHandler() = 0;
};

class MotorDrive {
private:
	State * currentState_;

public:
	MotorDrive(State * state) : currentState_(nullptr) {

	}

	~MotorDrive() {
		delete currentState_;
	}

	void ChangeState(State * state){
		if(this->currentState_ != nullptr){
			delete this->currentState_;
		}
		this->currentState_ = state;
		this->currentState_->setMotorDrive(this);
	}

	void Timer1InterruptHandler(){
		this->currentState_->Timer1InterruptHandler();
	}

	void DMA1InterruptHandler(){
		this->currentState_->DMA1InterruptHandler();
	}

	void DMA2InterruptHandler(){
		this->currentState_->DMA2InterruptHandler();
	}

};

class StoppedState : public State {
public:
	virtual void Timer1InterruptHandler() override;
	virtual void DMA1InterruptHandler() override;
	virtual void DMA2InterruptHandler() override;
};

class StartingState : public State {
public:
	virtual void Timer1InterruptHandler() override;
	virtual void DMA1InterruptHandler() override;
	virtual void DMA2InterruptHandler() override;
};


#endif /* INC_MOTORDRIVE_H_ */
