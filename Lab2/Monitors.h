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
created:	30:12:2014   15:33
filename: 	C:\Development\Projects\ParallelComputing\Lab2\Lab2\Threads.h
file path:	C:\Development\Projects\ParallelComputing\Lab2\Lab2
file base:	Threads
file ext:	h
author:		Horia Mut

purpose:	Contain declarations and prototypes for the threads.
*********************************************************************/
#pragma once
#include "stdafx.h"
#include "Definitions.h"

/*
Monitor Structure containing mutexes used.
@param n_CarsSouth : number of cars on the south lane.
@param n_CarsNorth : number of cars on the north lane.
@param mutex_CarsSouth : mutex to lock access to the south lane.
@param mutex_CarsNorth : mutex to lock access to the north lane.
@param cv_free_place : condition variable to say wether there is place for another vehicle in the tunnel or not.
*/
typedef struct
{
	int n_CarsSouth;
	pthread_mutex_t mutex_CarsSouth;
	pthread_cond_t cv_free_South;

	int n_CarsNorth;
	pthread_mutex_t mutex_CarsNorth;
	pthread_cond_t cv_free_North;

	int n_free_places;
	pthread_cond_t cv_free_place;
} MUTEX_FG;

typedef struct
{
	int ThreadID;
	int Location;

	MUTEX_FG* Mutex;

	int time_to_cross;
} CarData;

MUTEX_FG* mutex_Init(MUTEX_FG *m);

/*
* Car Threads.
* Each thread does all the work in order to be find out if there is still room in the Tunnel.
* Then verifies if the lane still has room for it and then is inserted in the correct waiting/crossing queue.
* If no data needs to be output for the user the code is simplified as there is no need for any queue nor mutexes.
* The Pthread semaphores are enough to control access to the tunnel and lanes.
*
* @param arg : pointer to void data containing the Structure used to contain the Location and ID of the thread.
*/
void* CarThreadN(void* arg);
void* CarThreadS(void* arg);

// CarThread critical sections.
void ct_CarNorth_entry(MUTEX_FG* m);
void ct_CarSouth_entry(MUTEX_FG* m);

void ct_CarNorth_exit(MUTEX_FG* m);
void ct_CarSouth_exit(MUTEX_FG* m);

/*
* Print Thread.
* This thread will display all traffic control data to the user.
* It prints the data every PRINT_EVERY_MS milliseconds (not taking in account console output time) then clears it.
* The thread never ends. Must be canceled by the user.
*/
void* PrintThread(void* arg);
