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

struct CarData
{
	int ThreadID;
	int Location;
};

/*
 * Car Thread.
 * Each thread does all the work in order to be find out if there is still room in the Tunnel.
 * Then verifies if the lane still has room for it and then is inserted in the correct waiting/crossing queue.
 * If no data needs to be output for the user the code is simplified as there is no need for any queue nor mutexes.
 * The Pthread semaphores are enough to control access to the tunnel and lanes.
 *
 * @param arg : pointer to void data containing the Structure used to contain the Location and ID of the thread.
 */
void* CarThread(void* arg);

/*
 * Print Thread.
 * This thread will display all traffic control data to the user.
 * It prints the data every PRINT_EVERY_MS milliseconds (not taking in account console output time) then clears it.
 * The thread never ends. Must be canceled by the user.
 */
void* PrintThread(void* arg);