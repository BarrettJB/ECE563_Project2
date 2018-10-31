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
	Predictor(unsigned int n, unsigned int m);
	void predict(unsigned long pcAddr);
	void print_state();
private:
	unsigned int *mGShare;
	unsigned int mPCBits;
	unsigned int mGlobal;
	unsigned int mGlobalMask;

	unsigned int shiftGlobal(unsigned int input, bool append);
};

#endif /* PREDICTOR_H_ */
