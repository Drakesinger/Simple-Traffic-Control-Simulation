#include "Monitors.h"
#include "SyncReadAndWrite.h"

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

void* CarThreadN(void* arg)
{
	// Get the Car Data.
	CarData* ThreadData = (CarData*)arg;
	int CarID = ThreadData->ThreadID;

	// Car is waiting.
	SyncAddToList(CarID, WaitingNorth, AcessWaitingNorth);

	// Try to enter the lane.
	CarThreadNorthEntry(ThreadData->Mutex);

	// Car entered the lane.
	SyncDelFromList(WaitingNorth, AcessWaitingNorth);

	// Add the car to the North-South traffic lane
	SyncAddToList(CarID, CrossingNorthSouth, AcessCrossingNorthSouth);

	// Crossing.
	Sleep(ThreadData->time_to_cross);

	// Reached the exit.
	CarThreadNorthExit(ThreadData->Mutex);

	// Delete a car from the list.
	SyncDelFromList(CrossingNorthSouth, AcessCrossingNorthSouth);

	// Cleanup.
	pthread_exit(NULL);
	return NULL;
}

void* CarThreadS(void* arg)
{
	// Get the Car Data.
	CarData* ThreadData = (CarData*)arg;
	int CarID = ThreadData->ThreadID;

	// Car is waiting.
	SyncAddToList(CarID, WaitingSouth, AcessWaitingSouth);

	// Try to enter the lane.
	CarThreadSouthEntry(ThreadData->Mutex);

	// Car entered the lane.
	SyncDelFromList(WaitingSouth, AcessWaitingSouth);

	// Add the car to the North-South traffic lane
	SyncAddToList(CarID, CrossingSouthNorth, AcessCrossingSouthNorth);

	// Crossing.
	Sleep(ThreadData->time_to_cross);

	// Reached the exit.
	CarThreadSouthExit(ThreadData->Mutex);

	// Delete a car from the list.
	SyncDelFromList(CrossingSouthNorth, AcessCrossingSouthNorth);

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

MonitorStruct* InitializeMonitor(MonitorStruct* Monitor)
{
	pthread_mutex_init(&Monitor->mutex_CarsSouth, NULL);
	pthread_mutex_init(&Monitor->mutex_CarsNorth, NULL);
	pthread_cond_init(&Monitor->cv_FreeSpot, NULL);
	pthread_cond_init(&Monitor->cv_FreeSouth, NULL);
	pthread_cond_init(&Monitor->cv_FreeNorth, NULL);

	// Maximum value is set in Definitions.h.
	Monitor->n_CarsSouth = 0;
	Monitor->n_CarsNorth = 0;

	// Max value of free room in the Tunnel.
	Monitor->n_free_places = MAX_CARS_IN_TUNNEL;

	return Monitor;
}

void CarThreadNorthEntry(MonitorStruct* Monitor)
{
	// Lock access to lane counter.
	pthread_mutex_lock(&Monitor->mutex_CarsNorth);

	// Check if we can enter.
	while (Monitor->n_free_places <= 0)
		pthread_cond_wait(&Monitor->cv_FreeSpot, &Monitor->mutex_CarsNorth);

	// Entering the north lane.
	Monitor->n_CarsNorth++;

	// Check if there is still room.
	while (Monitor->n_CarsNorth == MAX_CARS_ON_LANE)
		pthread_cond_wait(&Monitor->cv_FreeNorth, &Monitor->mutex_CarsNorth);

	// Entered the lane.
	Monitor->n_free_places--;

	pthread_mutex_unlock(&Monitor->mutex_CarsNorth);
}

void CarThreadNorthExit(MonitorStruct* Monitor)
{
	// Lock access to north lane counter.
	pthread_mutex_lock(&Monitor->mutex_CarsNorth);

	Monitor->n_CarsNorth--;
	if (Monitor->n_CarsNorth < MAX_CARS_ON_LANE)
	{
		// There is room on the lane.
		pthread_cond_broadcast(&Monitor->cv_FreeNorth);
	}

	// Car is out of the lane.
	Monitor->n_free_places++;
	pthread_cond_broadcast(&Monitor->cv_FreeSpot);
	// Unlock access to the lane counter.
	pthread_mutex_unlock(&Monitor->mutex_CarsNorth);
}

void CarThreadSouthEntry(MonitorStruct* Monitor)
{
	pthread_mutex_lock(&Monitor->mutex_CarsSouth);

	while (Monitor->n_free_places <= 0)
		pthread_cond_wait(&Monitor->cv_FreeSpot, &Monitor->mutex_CarsSouth);

	Monitor->n_CarsSouth++;

	while (Monitor->n_CarsSouth == MAX_CARS_ON_LANE)
		pthread_cond_wait(&Monitor->cv_FreeSouth, &Monitor->mutex_CarsSouth);

	Monitor->n_free_places--;

	pthread_mutex_unlock(&Monitor->mutex_CarsSouth);
}

void CarThreadSouthExit(MonitorStruct* Monitor)
{
	//pthread_mutex_lock(&m->mutex_Tunnel);
	pthread_mutex_lock(&Monitor->mutex_CarsSouth);

	Monitor->n_CarsSouth--;

	if (Monitor->n_CarsSouth < MAX_CARS_ON_LANE)
	{
		// Unlock other Cars from the south.
		pthread_cond_broadcast(&Monitor->cv_FreeSouth);
	}

	Monitor->n_free_places++;
	pthread_cond_broadcast(&Monitor->cv_FreeSpot);

	pthread_mutex_unlock(&Monitor->mutex_CarsSouth);
}