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

	ct_CarSouth_entry(ThreadData->Mutex);

	SyncDelFromList(WaitingSouth, AcessWaitingSouth);

	// Add the car to the North-South traffic lane
	SyncAddToList(CarID, CrossingSouthNorth, AcessCrossingSouthNorth);

	Sleep(ThreadData->time_to_cross);

	ct_CarSouth_exit(ThreadData->Mutex);

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

//////////////////////////////////////////////////////////////////////////
// Star Wars A New Hope
//////////////////////////////////////////////////////////////////////////

MUTEX_FG* mutex_Init(MUTEX_FG *m)
{
	pthread_mutex_init(&m->mutex_CarsSouth, NULL);
	pthread_mutex_init(&m->mutex_CarsNorth, NULL);
	pthread_cond_init(&m->cv_free_place, NULL);
	pthread_cond_init(&m->cv_free_South, NULL);
	pthread_cond_init(&m->cv_free_North, NULL);

	m->n_CarsSouth = 0;
	m->n_CarsNorth = 0;

	m->n_free_places = MAX_CARS_IN_TUNNEL;

	return m;
}

void ct_CarNorth_entry(MUTEX_FG *m)
{
	pthread_mutex_lock(&m->mutex_CarsNorth);

	while (m->n_free_places <= 0)
		pthread_cond_wait(&m->cv_free_place, &m->mutex_CarsNorth);

	m->n_CarsNorth++;

	while (m->n_CarsNorth == MAX_CARS_ON_LANE)
		pthread_cond_wait(&m->cv_free_North, &m->mutex_CarsNorth);

	m->n_free_places--;

	pthread_mutex_unlock(&m->mutex_CarsNorth);
}

void ct_CarNorth_exit(MUTEX_FG *m)
{
	pthread_mutex_lock(&m->mutex_CarsNorth);

	m->n_CarsNorth--;
	if (m->n_CarsNorth < MAX_CARS_ON_LANE)
	{
		pthread_cond_broadcast(&m->cv_free_North);
	}

	m->n_free_places++;
	pthread_cond_broadcast(&m->cv_free_place);

	pthread_mutex_unlock(&m->mutex_CarsNorth);
}

void ct_CarSouth_entry(MUTEX_FG *m)
{
	pthread_mutex_lock(&m->mutex_CarsSouth);

	while (m->n_free_places <= 0)
		pthread_cond_wait(&m->cv_free_place, &m->mutex_CarsSouth);

	m->n_CarsSouth++;

	while (m->n_CarsSouth == MAX_CARS_ON_LANE)
		pthread_cond_wait(&m->cv_free_South, &m->mutex_CarsSouth);

	m->n_free_places--;

	pthread_mutex_unlock(&m->mutex_CarsSouth);
}

void ct_CarSouth_exit(MUTEX_FG *m)
{
	pthread_mutex_lock(&m->mutex_CarsSouth);

	m->n_CarsSouth--;

	if (m->n_CarsSouth < MAX_CARS_ON_LANE)
	{
		// Unlock other Cars from the south.
		pthread_cond_broadcast(&m->cv_free_South);
	}

	m->n_free_places++;
	pthread_cond_broadcast(&m->cv_free_place);

	pthread_mutex_unlock(&m->mutex_CarsSouth);
}