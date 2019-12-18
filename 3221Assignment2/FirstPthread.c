#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *printToFile(void * arg);
pthread_cond_t start_line = PTHREAD_COND_INITIALIZER;
pthread_mutex_t filemut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t startmut = PTHREAD_MUTEX_INITIALIZER;
FILE *fp;

// number of times each thread prints
int M = 1000;
int main( int argc, char *argv[] )  {
	// N is the number of threads created
	int N = 100, i;

	// set N and M to be the user input otherwise they will stay with their
	// default values
	if (argc >= 2)
		N = atoi(argv[1]);
	if (argc >= 3)
		M = atoi(argv[2]);

	// create N different thread variables
	pthread_t *t = (pthread_t *)malloc(sizeof(pthread_t) * N);

	// create N different threads using the function printToFile
	for (i = 0; i < N; i++){
		if(pthread_create(&t[i], NULL, printToFile, NULL)) {
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}
	
	// open/create file
	fp = fopen("pthread_stats", "w");
	// activate the start line variable starting all the threads
	pthread_cond_broadcast( &start_line );
	
	// joins all the threads
	for (i = 0; i < N; i++)
		pthread_join(t[i], NULL);

	// close file, and return a valid execution of the main method
	fclose(fp);
	return 0;
}


void *printToFile(void * arg){
	// waits for the start_line condition to start
	pthread_cond_wait( &start_line, &startmut );
	// lets all threads continue that were waiting for start_line
	pthread_mutex_unlock( &startmut );

	int i;
	for (i = 0; i < M; i++){
		// only let one thread write to the file at a time using a mutex
		pthread_mutex_lock( &filemut );
		// write to the file
		fprintf(fp, "%ld\n", pthread_self());
		// allow another thread write to the file
		pthread_mutex_unlock( &filemut );
	}
}
