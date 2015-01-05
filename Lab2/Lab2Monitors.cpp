/*********************************************************************************
* Copyright (C) Haute-Ecole ARC - All Rights Reserved
*
* This file is part of <Concurrent Programming Lab # 2>.
*
* Unauthorized copying of this file, via any medium is strictly prohibited
* Proprietary and confidential
* Written by Horia Mut <horia.mut@net2000.ch>, 2014/12/30
**********************************************************************************/
/********************************************************************
created:	2014/12/30
created:	30:12:2014   14:58
filename: 	C:\Development\Projects\ParallelComputing\Lab2\Lab2\Lab2.cpp
file path:	C:\Development\Projects\ParallelComputing\Lab2\Lab2
file base:	Lab2
file ext:	cpp
author:		Horia Mut

purpose:	Implementation of a basic traffic control simulation
*********************************************************************/
// Lab2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Definitions.h"
#include "Monitors.h"

using namespace std;

// Mutexes to protect deque access.
pthread_mutex_t AcessWaitingNorth;
pthread_mutex_t AcessWaitingSouth;

pthread_mutex_t AcessCrossingNorthSouth;
pthread_mutex_t AcessCrossingSouthNorth;

// Global Variables
int CarsRemaining = MAX_CARS_TO_SPAWN;

int _tmain(int argc, _TCHAR* argv[])
{
	// Initialize the RNG seed.
	srand((unsigned)time(NULL));

	// Initialize Monitors
	MUTEX_FG Monitor;
	mutex_Init(&Monitor);

	// Initialize Mutexes.
	pthread_mutex_init(&AcessWaitingNorth, 0);
	pthread_mutex_init(&AcessWaitingSouth, 0);
	pthread_mutex_init(&AcessCrossingNorthSouth, 0);
	pthread_mutex_init(&AcessCrossingSouthNorth, 0);

	// Create the threads.
	pthread_t PrinterThread;
	pthread_t CarThreads[MAX_CARS_TO_SPAWN];
	// Create the Car Data structure array that will contain all the info for each car thread.
	CarData CarDataArray[MAX_CARS_TO_SPAWN];

	int ReturnCode;
	// Create print thread.
	ReturnCode = pthread_create(&PrinterThread, NULL, PrintThread, NULL);
	if (ReturnCode)
	{
		cout << "Could not create print thread.\n" << "Error code:" << ReturnCode << endl;
		return EXIT_FAILURE;
	}

	// Create Car threads.
	for (int iCars = 0; iCars < MAX_CARS_TO_SPAWN; iCars++)
	{
		int Location = rand() % 2; // North or South Entrance
		CarDataArray[iCars].ThreadID = iCars;
		CarDataArray[iCars].Location = Location;

		if (Location == LOC_NORTH)
		{
			ReturnCode = pthread_create(&CarThreads[iCars], NULL, CarThreadN, (void*)&CarDataArray[iCars]);
		}
		else
		{
			ReturnCode = pthread_create(&CarThreads[iCars], NULL, CarThreadS, (void*)&CarDataArray[iCars]);
		}

		if (ReturnCode)
		{
			cout << "Could not create thread.\n" << "Error code:" << ReturnCode << endl;
			return EXIT_FAILURE;
		}

		if (iCars % 2 == 1)
		{
			// Sleep after adding two vehicles.
			Sleep(TIME_BETWEEN_ARRIVALS_MS);
		}
		CarsRemaining--;
	}
	// Wait for the Printer Thread to finish (it never will).
	pthread_join(PrinterThread, NULL);

	pthread_exit(NULL);
	return EXIT_SUCCESS;
}