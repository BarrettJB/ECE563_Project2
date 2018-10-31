/*
 * Predictor.h
 *
 *  Created on: Oct 30, 2018
 *      Author: bryso
 */

#ifndef PREDICTOR_H_
#define PREDICTOR_H_

class Predictor {
public:
	Predictor(unsigned int m, unsigned int n);
	void predict(unsigned long pcAddr, bool taken);
	void print_state();
	void PrintStats();

private:
	unsigned int* mGShare;
	unsigned int mPCBits;
	unsigned int mPCMask;
	unsigned int mGlobal;
	unsigned int mGlobalBits;
	unsigned int mGlobalMask;
	unsigned int mGShareSize;

	//Stat Tracking
	int tCorrect;
	int tWrong;
	int tPredict;

	void UpdateTaken(unsigned int index);
	void UpdateNotTaken(unsigned int index);
	void initGShare();
	unsigned int shiftGlobal(unsigned int input, bool append);
	unsigned int getIndex(unsigned int pcAddr);
};

#endif /* PREDICTOR_H_ */
