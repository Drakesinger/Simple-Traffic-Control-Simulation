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
	created:	30:12:2014   15:31
	filename: 	C:\Development\Projects\ParallelComputing\Lab2\Lab2\SyncReadAndWrite.h
	file path:	C:\Development\Projects\ParallelComputing\Lab2\Lab2
	file base:	SyncReadAndWrite
	file ext:	h
	author:		Horia Mut

	purpose:	Tools to access STD Library containers in a thread-safe manner.
	*********************************************************************/
#pragma once
#include "stdafx.h"

using namespace std;

void SyncAddToList(int& Data, deque<int>& List, pthread_mutex_t& Mutex)
{
	// Lock access to the list.
	pthread_mutex_lock(&Mutex);
	// Add data to the list.
	List.push_back(Data);

	// Unlock access to the list.
	pthread_mutex_unlock(&Mutex);
}

void SyncDelFromList(deque<int>& List, pthread_mutex_t& Mutex)
{
	// Lock access to the list.
	pthread_mutex_lock(&Mutex);

	// Remove the first element from the list.
	List.pop_front();

	// Unlock access to the list.
	pthread_mutex_unlock(&Mutex);
}

void SyncReadFromList(deque<int>& List, pthread_mutex_t& Mutex)
{
	// Lock access to the waiting list.
	pthread_mutex_lock(&Mutex);

	int Size = List.size();
	for (int iList = 0; iList < Size; iList++)
	{
		cout << List.at(iList) << " ";
	}

	cout << "<" << Size << ">" << endl;
	// Unlock access to the list.
	pthread_mutex_unlock(&Mutex);
}