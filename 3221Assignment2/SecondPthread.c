#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *printRed(void * arg);
void *printGreen(void * arg);

pthread_cond_t green_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t red_cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t filemut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t redmut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t greenmut = PTHREAD_MUTEX_INITIALIZER;
FILE *fp;
int N = 100 ,M = 1000, L = 10, counter = 0;
int main( int argc, char *argv[] )  {

	// sets the values of N, M, and L to the input values
	if (argc >= 2)
		N = atoi(argv[1]);
	if (argc >= 3)
		M = atoi(argv[2]);
	if (argc >= 4)
		L = atoi(argv[3]);

	// allocates memory for 2 * N pthreads
	pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t) * (2 * N));

	int i;
	// creates N red and green threads
	for (i = 0; i < N; i++){
		if(pthread_create(&t[i], NULL, printRed, NULL)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
		if(pthread_create(&t[N + i], NULL, printGreen, NULL)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}

	// opens the file for writing
	fp = fopen("pthread_stats", "w");

	// broadcasts a message for all waiting threads for the red_cond condition variable
	pthread_cond_broadcast( &red_cond );
	
	// joins all the threads
	for (i = 0; i < 2 * N; i++)
		pthread_join(t[i], NULL);

	// close the file and return with a 0 value
	fclose(fp);
	return 0;
}

// method for red threads
void *printRed(void * arg){
	int g;
	// do the function L times
	for (g = 0; g < L; g++){
		// wait for the red condition variable to be signalled
		pthread_cond_wait( &red_cond, &redmut );
		// let all other red threads continue now
		pthread_mutex_unlock( &redmut );
		int i;
		// do this M times
		for (i = 0; i < M; i++){
			// let only one thread write to the file at a time
			pthread_mutex_lock( &filemut );
			fprintf(fp, "%ld	red\n", pthread_self());
			pthread_mutex_unlock( &filemut );
		}
		// add 1 to the counter
		counter++;
		// if the counter is equal to the number of red processes
		if (counter == N){
			// set the counter to 0 and broadcast to all the green threads waiting
			counter = 0;
			pthread_cond_broadcast( &green_cond );
		}
	}
}

// same, but with red being green and green being red
void *printGreen(void * arg){
	int g;
	for (g = 0; g < L; g++){
		pthread_cond_wait( &green_cond, &greenmut );
		pthread_mutex_unlock( &greenmut );
		int i;
		for (i = 0; i < M; i++){
			pthread_mutex_lock( &filemut );
			fprintf(fp, "%ld	green\n", pthread_self());
			pthread_mutex_unlock( &filemut );
		}
		counter++;
		if (counter == N){
			counter = 0;
			pthread_cond_broadcast( &red_cond );
		}
	}
}
