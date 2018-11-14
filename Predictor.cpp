/*
 * Predictor.cpp
 *
 *  Created on: Oct 30, 2018
 *      Author: bryso
 */

#include "Predictor.h"
#include <iostream>
#include <stdio.h>
#include <algorithm>

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
	mGShareSize = std::max(mSize,nSize);
	mGShare = new unsigned int[mGShareSize];

	if(VERBOSE) {

		std::cout << "Created New Predictor..." << std::endl
				  << " PCBits: " << mPCBits << std::endl
				  << " PCMask: " << mPCMask << std::endl
				  << " GlobalBits: " << mGlobalBits << std::endl
				  << " GlobalMask: " << mGlobalMask << std::endl
				  << " GShareSize: " << mGShareSize << std::endl;
	}

	tCorrect = 0;
	tWrong = 0;
	tPredict = 0;

	initGShare();
}

bool Predictor::predict(unsigned long pcAddr, bool taken) {
	tPredict++;
	if (VERBOSE) std::cout << std::endl << "Address: " << pcAddr << std::endl;
	unsigned int index = getIndex(pcAddr);

	if(VERBOSE) std::cout << "Index: " << index << std::endl;
    if(VERBOSE) std::cout << "Prediction: " << mGShare[index] << std::endl;

	if (mGShare[index] >= 2)
	{
		if (taken) {
			//predicted correctly
			tCorrect++;
			if(VERBOSE) std::cout << "Predicted Taken Correctly" << std::endl;
		}
		else
		{
			//predicted wrong
			tWrong++;
			if(VERBOSE) std::cout << "Predicted Taken Wrongly" << std::endl;
		}
		return true;
	}
	else
	{
		if(!taken)
		{
			//predicted correctly
			tCorrect++;
			if(VERBOSE) std::cout << "Predicted Not Taken Correctly" << std::endl;
		}
		else
		{
			//predicted wrong
			tWrong++;
			if(VERBOSE) std::cout << "Predicted Not Taken Wrongly" << std::endl;
		}
		return false;
	}
}

void Predictor::UpdateTaken(unsigned int addr){
	unsigned int index = getIndex(addr);
	//Update Predictor
	mGShare[index] = mGShare[index] + 1;
	//Saturation
	if (mGShare[index] > 3)
		mGShare[index] = 3;

	//Update global counter
	mGlobal = (mGlobal >> 1) + (1 << (mGlobalBits - 1));
	if(VERBOSE) std::cout << "Global Counter: " << mGlobal << std::endl;
}

void Predictor::UpdateNotTaken(unsigned int addr){
	unsigned int index = getIndex(addr);
	//Saturation
	if (mGShare[index] == 0)
		mGShare[index] = 1;
	//Update Predictor
	mGShare[index]--;

	//Update global counter
	mGlobal = (mGlobal >> 1) + 0;
}

void Predictor::UpdateGlobalHistory(bool history) {
	if(history) {
		mGlobal = (mGlobal >> 1) + (1 << (mGlobalBits - 1));
	}
	else {
		mGlobal = (mGlobal >> 1) + 0;
	}
}

void Predictor::SetChooser() {
	for(unsigned int i = 0; i < mGShareSize; i++) {
		mGShare[i] = 1;
	}
}

unsigned Predictor::getIndex(unsigned int pcAddr) {
	unsigned int index = ((pcAddr >> 2) & mPCMask) ^ ((mGlobal & mGlobalMask) << (mPCBits-mGlobalBits));
	return index;
}

void Predictor::initGShare () {
	for(unsigned int i = 0; i < mGShareSize; i++) {
		mGShare[i] = 2;
	}
}

void Predictor::PrintStats(){
	float miss_rate = (100.0*tWrong)/tPredict;
	std::cout << " number of predictions:       " << tPredict << std::endl;
	std::cout << " number of mispredictions:    " << tWrong << std::endl;
	printf(" misprediction rate:          %2.2f", miss_rate);
}

void Predictor::PrintContents() {
	for(unsigned int i = 0; i < mGShareSize; i++) {
		std::cout << " " << i << "  " << mGShare[i] << std::endl;
	}
}
