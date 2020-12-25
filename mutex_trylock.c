/*
Name: Prasad Hajare
Program: Learning functionality of trylock over a critical section using 2 threads
Instructions to run the code: gcc assignment3.c -lpthread ./a.out

Sample output:

Synchronization using trylock:
Thread1 Update_by_100 ***12483*** times
Thread1 contributed for: 2500000
Thread2 contributed for: 2500000
Final Value: 5000000
Time used is sec: 1, usec 336702
System Time used is sec: 0, usec 1987

Synchronization using binary semaphore:
Thread1 Update_by_100 ***12609*** times
Thread1 contributed for: 2500000
Thread2 contributed for: 2500000
Final Value: 5000000
Time used is sec: 0, usec 481093
System Time used is sec: 0, usec 339602

Observation: System time used is more in case of binary semaphores because when any one of
the threads aquires the lock, value of semaphore varible becomes 0 (intial value 1).
wait() function of semaphore has while loop which checks the value if it is less than or equal to 0.
And it consumes some cycles and eventually it adds to the execution time.
*/

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/resource.h>

struct rusage mytiming;
struct timeval mytimeval;

pthread_t T1, T2, T3;

unsigned long counter1 = 0;
unsigned long counter2 = 0;
unsigned long counter3 = 0;

sem_t mutex;

unsigned long value = 0;
int hundred = 0;

void* update1(void* arg)
{
	while (counter1 < 2500000) {							                // counter1: update to value contributed by thread1
		if (value % 100 == 0 && counter1 <= 2499900){
			sem_wait(&mutex);								// lock
			value = value + 100;							        // incrementing value by 100
			counter1 += 100;								// incrementing contribution of thread1 by 100
			hundred += 1;									// no. of 100 updates
			sem_post(&mutex);								// release lock
		}

		if (counter1 < 2500000){
			sem_wait(&mutex);								// lock
			value = value + 1;								// incrementing value by 1
			counter1 += 1;									// incrementing contribution of thread2 by 1
			sem_post(&mutex);								// release lock
		}
	}

	return NULL;
}


void* update2(void* arg)
{
	while (counter2 < 2500000) {							                // counter2: update to value contributed by thread2
		sem_wait(&mutex);									// lock
		value = value + 1;									// incrementing value by 1
		counter2 += 1;										// incrementing contribution by thread2 by 1
		sem_post(&mutex);									// release lock
	}
	return NULL;
}

void* update3(void* arg)
{
	while (counter3 < 2500000) {							                // counter3: update to value contributed by thread3
		sem_wait(&mutex);									// lock
		value = value + 1;									// incrementing value by 1
		counter3 += 1;										// incrementing contribution by thread2 by 1
		sem_post(&mutex);									// release lock
	}
	return NULL;
}


void main()
{
	int thread1_fail, thread2_fail, thread3_fail;							// variables to check thread initialization failure

	if (sem_init(&mutex, 0, 1))								        // semaphore initialization
		printf("\n Mutex Initialization Failed\n");

	thread1_fail = pthread_create(&T1, NULL, &update1, NULL); // thread1 creation
	if (thread1_fail)
		printf("\nThread1 creation failed\n");
	thread2_fail = pthread_create(&T2, NULL, &update2, NULL); // thread2 creation
	if (thread2_fail)
		printf("\nThread2 creation failed\n");
	thread3_fail = pthread_create(&T3, NULL, &update3, NULL); // thread3 creation
	if (thread3_fail)
		printf("\nThread3 creation failed\n");

	pthread_join(T1, NULL);
	pthread_join(T2, NULL);
	pthread_join(T3, NULL);
	sem_destroy(&mutex);							                        // destroy semaphore
	printf("Thread1 Update_by_100 ***%d*** times\n", hundred);
	printf("Thread1 contributed for: %d\n", counter1);
	printf("Thread2 contributed for: %d\n", counter2);
	printf("Thread3 contributed for: %d\n", counter3);

	printf("Final Value: %d\n", value);

	getrusage(RUSAGE_SELF, &mytiming);
	printf("Time used is sec: %d, usec %d\n",mytiming.ru_utime.tv_sec,mytiming.ru_utime.tv_usec);
	printf("System Time used is sec: %d, usec %d\n",mytiming.ru_stime.tv_sec,mytiming.ru_stime.tv_usec);

}
