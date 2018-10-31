/*
 * Predictor.cpp
 *
 *  Created on: Oct 30, 2018
 *      Author: bryso
 */

#include "Predictor.h"

Predictor::Predictor(unsigned int n, unsigned int m) {
	unsigned int nSize = 1 << n;
	unsigned int mSize = 1 << m;
	mPCBits = n;
	mGlobalMaks = mSize - 1;

}
