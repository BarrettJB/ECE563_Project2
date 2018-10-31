#include <iostream>
#include "Predictor.h"

int main (int argc, char* argv[])
{
	std::cout << "Starting Branch Predictor" << std::endl;
	Predictor p(2,0);

	for (int i = 0; i < 10; i++)
	{
		p.predict(0,true);
	}

	p.PrintStats();
}
