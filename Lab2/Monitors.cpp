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
	int Location = ThreadData->Location;

	SyncAddToList(CarID, WaitingNorth, AcessWaitingNorth);

	ct_CarNorth_entry(ThreadData->Mutex);

	SyncDelFromList(WaitingNorth, AcessWaitingNorth);

	// Add the car to the North-South traffic lane
	SyncAddToList(CarID, CrossingNorthSouth, AcessCrossingNorthSouth);

	Sleep(ThreadData->time_to_cross);

	ct_CarNorth_exit(ThreadData->Mutex);

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
	int Location = ThreadData->Location;

	SyncAddToList(CarID, WaitingSouth, AcessWaitingSouth);

	ct_CarNorth_entry(ThreadData->Mutex);

	SyncDelFromList(WaitingSouth, AcessWaitingSouth);

	// Add the car to the North-South traffic lane
	SyncAddToList(CarID, CrossingSouthNorth, AcessCrossingSouthNorth);

	Sleep(ThreadData->time_to_cross);

	ct_CarNorth_exit(ThreadData->Mutex);

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