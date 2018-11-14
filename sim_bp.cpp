#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <iomanip>
#include "sim_bp.h"
#include "Predictor.h"

#define VERBOSE false

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim bimodal 6 gcc_trace.txt
    argc = 4
    argv[0] = "sim"
    argv[1] = "bimodal"
    argv[2] = "6"
    ... and so on
 */
int main (int argc, char* argv[])
{
	FILE *FP;               // File handler
	char *trace_file;       // Variable that holds trace file name;
	bp_params params;       // look at sim_bp.h header file for the the definition of struct bp_params
	char outcome;           // Variable holds branch outcome
	unsigned long int addr; // Variable holds the address read from input file

	if (!(argc == 4 || argc == 5 || argc == 7))
	{
		printf("Error: Wrong number of inputs:%d\n", argc-1);
		exit(EXIT_FAILURE);
	}

	params.bp_name  = argv[1];

	// strtoul() converts char* to unsigned long. It is included in <stdlib.h>
	if(strcmp(params.bp_name, "bimodal") == 0)              // Bimodal
	{
		if(argc != 4)
		{
			printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
			exit(EXIT_FAILURE);
		}
		params.M2       = strtoul(argv[2], NULL, 10);
		trace_file      = argv[3];
		printf("COMMAND\n %s %s %lu %s\n", argv[0], params.bp_name, params.M2, trace_file);
	}
	else if(strcmp(params.bp_name, "gshare") == 0)          // Gshare
	{
		if(argc != 5)
		{
			printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
			exit(EXIT_FAILURE);
		}
		params.M1       = strtoul(argv[2], NULL, 10);
		params.N        = strtoul(argv[3], NULL, 10);
		trace_file      = argv[4];
		printf("COMMAND\n %s %s %lu %lu %s\n", argv[0], params.bp_name, params.M1, params.N, trace_file);

	}
	else if(strcmp(params.bp_name, "hybrid") == 0)          // Hybrid
	{
		if(argc != 7)
		{
			printf("Error: %s wrong number of inputs:%d\n", params.bp_name, argc-1);
			exit(EXIT_FAILURE);
		}
		params.K        = strtoul(argv[2], NULL, 10);
		params.M1       = strtoul(argv[3], NULL, 10);
		params.N        = strtoul(argv[4], NULL, 10);
		params.M2       = strtoul(argv[5], NULL, 10);
		trace_file      = argv[6];
		printf("COMMAND\n %s %s %lu %lu %lu %lu %s\n", argv[0], params.bp_name, params.K, params.M1, params.N, params.M2, trace_file);

	}
	else
	{
		printf("Error: Wrong branch predictor name:%s\n", params.bp_name);
		exit(EXIT_FAILURE);
	}

	// Open trace_file in read mode
	FP = fopen(trace_file, "r");
	if(FP == NULL)
	{
		// Throw error and exit if fopen() failed
		printf("Error: Unable to open file %s\n", trace_file);
		exit(EXIT_FAILURE);
	}


	//Create Branch Predictor based on arguments
	Predictor *bp;
	Predictor *bi;
	Predictor *gs;
	bool bimodal;
	bool gshare;
	bool hybrid;
	int tWrong;
	int tPredict;
	if(strcmp(params.bp_name, "bimodal") == 0) {
		bp = new Predictor(params.M2, 0);
		bimodal = true;
		gshare = false;
		hybrid = false;
	}
	if(strcmp(params.bp_name, "gshare") == 0){
		bp = new Predictor(params.M1, params.N);
		bimodal = false;
		gshare = true;
		hybrid = false;
	}
	if(strcmp(params.bp_name, "hybrid") == 0) {
		bp = new Predictor(params.K, 0);
		bp->SetChooser();
		bi = new Predictor(params.M2, 0);
		gs = new Predictor(params.M1, params.N);
		bimodal = false;
		gshare = false;
		hybrid = true;
		tWrong = 0;
		tPredict = 0;
	}

	char str[2];
	bool taken = false;
	while(fscanf(FP, "%lx %s", &addr, str) != EOF)
	{

		outcome = str[0];
		if (outcome == 't')
		{
			//printf("%lx %s\n", addr, "t");           // Print and test if file is read correctly
			taken = true;
		}
		else if (outcome == 'n')
		{
			//printf("%lx %s\n", addr, "n");          // Print and test if file is read correctly
			taken = false;
		}

		if (hybrid) {
			//Get both predictions
			tPredict++;
			bool gs_ret = gs->predict(addr,taken);
			bool bi_ret = bi->predict(addr,taken);

			//Select the correct predictor and update
			if(bp->predict(addr,taken))
			{
				if(VERBOSE) std::cout << "Selected the GShare Predictor" << std::endl;
				if(taken) gs->UpdateTaken(addr);
				else gs->UpdateNotTaken(addr);
				if (gs_ret != taken) tWrong++;
			}
			else
			{
				if(VERBOSE) std::cout << "Selected the Bimodal Predictor" << std::endl;
				if(taken) bi->UpdateTaken(addr);
				else bi->UpdateNotTaken(addr);
				gs->UpdateGlobalHistory(taken);
				if (bi_ret != taken) tWrong++;
			}

			//Update hybrid values
			if ((gs_ret == taken) && (bi_ret != taken))
			{
				if(VERBOSE) std::cout << "Biased toward GShare" << std::endl;
				bp->UpdateTaken(addr);
			}
			if ((gs_ret != taken) && (bi_ret == taken))
			{
				if(VERBOSE) std::cout << "Biased Toward Bimodal" << std::endl;
				bp->UpdateNotTaken(addr);
			}
		}
		else
		{
			bp->predict(addr,taken);

			if (taken) {
				bp->UpdateTaken(addr);
			}
			else {
				bp->UpdateNotTaken(addr);
			}
		}

	}
    std::cout << std::setprecision(4);
	std::cout << "OUTPUT" << std::endl;
	if(hybrid) {
		float miss_rate = (100.0*tWrong)/tPredict;
		std::cout << " number of predictions:       " << tPredict << std::endl;
		std::cout << " number of mispredictions:    " << tWrong << std::endl;
		printf(" misprediction rate:          %2.2f%%\n", miss_rate);
	}
	else {
	    bp->PrintStats();
	}

	if (bimodal){
		std::cout << "FINAL BIMODAL CONTENTS" << std::endl;
		bp->PrintContents();
	}

	if (gshare){
		std::cout << "FINAL GSHARE CONTENTS" << std::endl;
		bp->PrintContents();
	}

	if (hybrid) {
		std::cout << "FINAL CHOOSER CONTENTS" << std::endl;
		bp->PrintContents();
		std::cout << "FINAL GSHARE CONTENTS" << std::endl;
	    gs->PrintContents();
		std::cout << "FINAL BIMODAL CONTENTS" << std::endl;
		bi->PrintContents();
	}
	return 0;
}
