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
	created:	30:12:2014   15:29
	filename: 	C:\Development\Projects\ParallelComputing\Lab2\Lab2\Threads.cpp
	file path:	C:\Development\Projects\ParallelComputing\Lab2\Lab2
	file base:	Threads
	file ext:	cpp
	author:		Horia Mut

	purpose:	Define the code for the two threads that exist in the program.
	*********************************************************************/
#include "stdafx.h"
#include "Threads.h"
#include "Definitions.h"
#include "SyncReadAndWrite.h"

// Extern Semaphores
extern sem_t SemaphoreNorth;
extern sem_t SemaphoreSouth;
extern sem_t SemaphoreAccess;

// Extern Mutexes to protect deque access.
extern pthread_mutex_t AcessWaitingNorth;
extern pthread_mutex_t AcessWaitingSouth;

extern pthread_mutex_t AcessCrossingNorthSouth;
extern pthread_mutex_t AcessCrossingSouthNorth;

// Extern Global Variables
extern int CarsRemaining;

// Double-ended queues containing the Car IDs for each car at a specific location.
deque<int> WaitingNorth;
deque<int> WaitingSouth;
deque<int> CrossingNorthSouth;
deque<int> CrossingSouthNorth;

void* CarThread(void* arg)
{
	// Get the Car Data.
	struct CarData* ThreadData = (struct CarData*) arg;
	int CarID = ThreadData->ThreadID;
	int Location = ThreadData->Location;

	// Check if there is still room in the tunnel to go through.
	sem_wait(&SemaphoreAccess);

	// Check at which of the tunnel entrances the car is.
	if (Location == LOC_NORTH)
	{
		SyncAddToList(CarID, WaitingNorth, AcessWaitingNorth);

		// Now check if there is room in the tunnel to go through.
		sem_wait(&SemaphoreNorth);

		SyncDelFromList(WaitingNorth, AcessWaitingNorth);

		// Add the car to the North-South traffic lane
		SyncAddToList(CarID, CrossingNorthSouth, AcessCrossingNorthSouth);

		// Car is crossing.
		Sleep(TIME_TO_CROSS_MS);

		// Car crossed.
		// Increment the semaphore value.
		sem_post(&SemaphoreNorth);
		// Delete a car from the list.
		SyncDelFromList(CrossingNorthSouth, AcessCrossingNorthSouth);
	}
	else
	{
		SyncAddToList(CarID, WaitingSouth, AcessWaitingSouth);

		// Now check if there is room in the tunnel to go through.
		sem_wait(&SemaphoreSouth);

		SyncDelFromList(WaitingSouth, AcessWaitingSouth);

		// add the car to the South-North traffic lane
		SyncAddToList(CarID, CrossingSouthNorth, AcessCrossingSouthNorth);

		// Car is crossing.
		Sleep(TIME_TO_CROSS_MS);

		// Car crossed.
		// Unlock the semaphore and remove the car from the queues.
		sem_post(&SemaphoreSouth);
		SyncDelFromList(CrossingSouthNorth, AcessCrossingSouthNorth);
	}

	// Car crossed.
	sem_post(&SemaphoreAccess);

	// Cleanup.
	pthread_exit(NULL);
	return NULL;
}


void* PrintThread(void* arg)
{
	while (true)
	{
		cout << "Cars remaining: " << CarsRemaining << endl;

		cout << "Waiting  North entry:";
		SyncReadFromList(WaitingNorth, AcessWaitingNorth);

		cout << "Crossing North-South:";
		SyncReadFromList(CrossingNorthSouth, AcessCrossingNorthSouth);

		cout << "Waiting  South entry:";
		SyncReadFromList(WaitingSouth, AcessWaitingSouth);

		cout << "Crossing South-North:";
		SyncReadFromList(CrossingSouthNorth, AcessCrossingSouthNorth);

		// Sleep so that the user can actually see the data.
		Sleep(PRINT_EVERY_MS);

// Clear the console. MS Specific.
#ifdef _WIN32
		system("cls");
#endif // _WIN32

// Linux specific. Redundant code as the application is compiled for Windows platform.
#ifdef __linux__
		system("clear");
#endif // __linux__
	}

	pthread_exit(NULL);
	return NULL;
}