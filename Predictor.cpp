/*
 * Predictor.cpp
 *
 *  Created on: Oct 30, 2018
 *      Author: bryso
 */

#include "Predictor.h"
#include <iostream>
#include <stdio.h>

//TODO clean up debug messages?
#define VERBOSE false

Predictor::Predictor(unsigned int m, unsigned int n) {
	unsigned int nSize = 1 << n;
	unsigned int mSize = 1 << m;

	mPCBits = m;
	mPCMask = mSize - 1;
	mGlobal = 0;
	mGlobalBits = n;
	mGlobalMask = nSize - 1;
	mGShareSize = mSize*nSize;
	std::cout << "Size: " << mGShareSize << std::endl;
	mGShare = new unsigned int[mGShareSize];

	tCorrect = 0;
	tWrong = 0;
	tPredict = 0;

	initGShare();
}

void Predictor::predict(unsigned long pcAddr, bool taken) {
	tPredict++;
	unsigned int index = getIndex(pcAddr);

	if(VERBOSE) std::cout << "Index: " << index << std::endl;
	if(VERBOSE) std::cout << "Prediction: " << mGShare[index] << std::endl;

	if (mGShare[index] >= 3)
	{
		if (taken) {
			//predicted correctly
			tCorrect++;
			UpdateTaken(index);
			if(VERBOSE) std::cout << "Predicted Taken Correctly" << std::endl;
		}
		else
		{
			//predicted wrong
			tWrong++;
			UpdateNotTaken(index);
			if(VERBOSE) std::cout << "Predicted Taken Wrongly" << std::endl;
		}
	}
	else
	{
		if(!taken)
		{
			//predicted correctly
			tCorrect++;
			UpdateNotTaken(index);
			if(VERBOSE) std::cout << "Predicted Not Taken Correctly" << std::endl;
		}
		else
		{
			//predicted wrong
			tWrong++;
			UpdateTaken(index);
			if(VERBOSE) std::cout << "Predicted Not Taken Wrongly" << std::endl;
		}
	}
}

void Predictor::UpdateTaken(unsigned int index){
	//Update Predictor
	mGShare[index] = mGShare[index] + 1;
	//Saturation
	if (mGShare[index] > 4)
		mGShare[index] = 4;

	//Update global counter
	mGlobal = (mGlobal << 1) + 1;
}

void Predictor::UpdateNotTaken(unsigned int index){
	//Update Predictor
	//Saturation
	if (mGShare[index] == 0)
		mGShare[index] = 1;
	mGShare[index]--;

	//Update global counter
	mGlobal = (mGlobal << 1) + 0;
}

unsigned Predictor::getIndex(unsigned int pcAddr) {
	unsigned int index = ((pcAddr >> 2) & mPCMask) ^ ((mGlobal & mGlobalMask) << (mPCBits-mGlobalBits));
	unsigned int val = (pcAddr >> 2) & mPCMask;
	if (mGlobalBits > 0)
		val = val * mGlobalBits + (mGlobal & mGlobalMask);
	return val;
}

void Predictor::initGShare () {
	for(unsigned int i = 0; i < mGShareSize; i++) {
		mGShare[i] = 2;
	}
}

void Predictor::PrintStats(){
	std::cout << "Total Predictions:    " << tPredict << std::endl;
	std::cout << "Wrong Predictions:    " << tWrong << std::endl;
	std::cout << "Correct Predictions:  " << tCorrect << std::endl;
}
