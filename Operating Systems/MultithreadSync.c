#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/* the maximum time (in seconds) to sleep */
#define MAX_SLEEP_TIME 3
/* number of students */
#define NUM_OF_STUDENTS 5
/* number of TAs */
#define NUM_OF_TAS 2
/* # of helps each student must receive before terminates */
#define NUM_OF_HELPS 2
/* number of available seats */
#define NUM_OF_SEATS 2

/* semaphores and mutex lock */
pthread_mutex_t mutex_lock;

/* the number of waiting students */
int waiting_students;

/* semaphore declarations */
sem_t students_sem; /*wait: ta waits for a student to show up, signal: student notifies ta his/her arrival*/
sem_t ta_sem; /*wait: student waits for ta to help, signal: ta notifies student he/she is ready to help*/

void * ta(void * num) {
	int i = *((int *) num);
	unsigned int seed = time(NULL) + i;
	int sleepTime;
	int helped = 0;
	while (1) {
		sleepTime = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
		sem_wait(&students_sem);
		sem_post(&ta_sem);
		pthread_mutex_lock(&mutex_lock);
		waiting_students--;
		pthread_mutex_unlock(&mutex_lock);
		printf(
				"TA[%d, %d] helps a student for %d seconds, # of waiting students = %d\n",
				i, ++helped, sleepTime, waiting_students);
		fflush(NULL);
		sleep(sleepTime);
	}
}

void * student(void * num) {
	int i = *((int *) num);
	unsigned int seed = time(NULL) + i;
	int sleepTime;
	int helped = 0;
	while (helped < NUM_OF_HELPS) {
		sleepTime = (rand_r(&seed) % MAX_SLEEP_TIME) + 1;
		printf("student[%d, %d] programs for %d seconds\n", i, helped, sleepTime);
		fflush(NULL);
		sleep(sleepTime);
		pthread_mutex_lock(&mutex_lock);
		if (waiting_students < NUM_OF_SEATS) {
			waiting_students++;
			pthread_mutex_unlock(&mutex_lock);
			printf("student[%d, %d] takes a seat, # of waiting students = %d\n",
					i, helped, waiting_students);
			fflush(NULL);
			sem_post(&students_sem);
			sem_wait(&ta_sem);
			printf("student[%d, %d] receives help\n", i, ++helped);
			fflush(NULL);
		} else {
			pthread_mutex_unlock(&mutex_lock);
			printf("student[%d, %d] will try again later\n", i, helped);
			fflush(NULL);
		}
	}
	pthread_exit((void *) 0);
}

int main() {
	printf("CS149 SleepingTA from Lovejit Kharod\n\n");
	fflush(NULL);
	pthread_t thread_id[NUM_OF_STUDENTS + NUM_OF_TAS];
	waiting_students = 0;

	// initialize the semaphores and mutex
	sem_init(&students_sem, 0, 0);
	sem_init(&ta_sem, 0, 0);
	pthread_mutex_init(&mutex_lock, NULL);

	for (int i = 0; i < NUM_OF_STUDENTS; i++) {
		int * x = malloc(sizeof(*x));
		*x = i;
		pthread_create(&thread_id[i], NULL, student, x);
	}

	for (int i = NUM_OF_STUDENTS; i < NUM_OF_STUDENTS + NUM_OF_TAS; i++) {
		// create student processes
		int * x = malloc(sizeof(*x));
		*x = i - NUM_OF_STUDENTS;
		pthread_create(&thread_id[i], NULL, ta, x);
	}

	for (int i = 0; i < NUM_OF_STUDENTS; i++)
		pthread_join(thread_id[i], NULL);
	for (int i = NUM_OF_STUDENTS; i < NUM_OF_STUDENTS + NUM_OF_TAS; i++)
		pthread_cancel(thread_id[i]);

	printf("main: done\n");
	fflush(NULL);

	sem_destroy(&students_sem);
	sem_destroy(&ta_sem);
	pthread_mutex_destroy(&mutex_lock);
}
