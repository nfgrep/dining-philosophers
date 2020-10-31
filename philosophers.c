/**
* Dining philosophers problem
* Solved using mutex and condition variable
* Nathan Faber-Good 2020.10.30
* Assignment 2
* SYST30102
*
* run with optional arg <run_time>
* if no argument, will run for 10 seconds
**/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_PHILO 5

pthread_mutex_t mutex;
pthread_cond_t cond_var;

/* Forks represented as 1:available 0:in use*/
char forks[NUM_PHILO];

void pickup_forks(int philo_id)
{
	/* Acquire mutex lock */
	pthread_mutex_lock(&mutex);

	/* Wait until both forks are available */
	while(!forks[philo_id] || !forks[(philo_id+1) % NUM_PHILO])
		pthread_cond_wait(&cond_var,&mutex);

	/* Use the forks, making them unavailable */
	forks[philo_id] = 0;
	forks[(philo_id+1) % NUM_PHILO] = 0;
	printf("philosopher %d picked up their forks. ",philo_id);
	printf("forks: %d%d%d%d%d\n",forks[0],forks[1],forks[2],forks[3],forks[4]);

	/* Release mutex lock */
	pthread_mutex_unlock(&mutex);		
}

void return_forks(int philo_id)
{
	/* Acquire mutex lock */
	pthread_mutex_lock(&mutex);

	/* Make forks available */
	forks[philo_id] = 1;
	forks[(philo_id+1) % NUM_PHILO] = 1;
	printf("philosopher %d returned their forks.  ",philo_id);
	printf("forks: %d%d%d%d%d\n",forks[0],forks[1],forks[2],forks[3],forks[4]);
	
	/* Notify other threads with condition variable */
	pthread_cond_broadcast(&cond_var);

	/* Release mutex lock */
	pthread_mutex_unlock(&mutex);
}

void * philosopher(void * philo_id)
{
	int id = *((int *) philo_id);
	while(1)
	{
		/* Time to think */
		sleep((rand() % 3));
		pickup_forks(id);
		
		/* Time to sleep */
		sleep((rand() % 3));
		return_forks(id);
	}
}

int main(int argc, char* argv[])
{
	/* Getting optional <run_time> arg */
	int run_time = 10;
	if(argc > 2)
	{
		printf("Usage:./philo <run_time>(optional)\n");
		return 1;
	}
	else if(argc == 2)
		run_time = atoi(argv[1]);

	/* Initializing forks to 1 or 'true' */
	int i;
	for(i = 0; i < NUM_PHILO; i++)
		forks[i] = 1;
	
	/* Creating threads for each pholosopher */
	void* ph_id;
	for(i = 0; i < NUM_PHILO; i++)
	{
		pthread_t tid;
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		/* Avoid memory hazard of passing local stack var to thread */
		ph_id =  malloc(sizeof(int));
		*((int*)ph_id) = i;
		pthread_create(&tid, &attr, philosopher,ph_id);
	}

	/* Allow threads to run before exit */
	sleep(run_time);

	return 0;
}
