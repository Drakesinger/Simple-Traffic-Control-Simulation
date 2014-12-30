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

void* CarThread(void* arg);
void* PrintThread(void* arg);