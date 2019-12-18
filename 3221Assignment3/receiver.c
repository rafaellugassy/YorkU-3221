/* Assumption that make will be called before every use of sender and receiver */

#include <stdio.h>
#include <stdlib.h> 
#include <signal.h> 
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

void handle_signal(int signal);
int i;
char *map;

int main() {
	// create a sigaction variable
	struct sigaction action;

	// print the pid so the user can type it in when they execute sender
	printf("pid = %d\n", getpid());
	
	i = 0;

	// open the file
	int fd = open("sharedfile", O_RDONLY, (mode_t)0400);
	if(fd == -1){
		fprintf(stderr, "error could not open file");
		exit(1);
	}

	// open the memory map and save it to map
	map = mmap(0, 100, PROT_READ, MAP_SHARED, fd, 0);

	// apply attributes to the action
	action.sa_handler = &handle_signal;
	action.sa_flags = SA_RESTART;
	sigfillset(&action.sa_mask);

	// apply an action for SIGTERM
	if (sigaction(SIGTERM, &action, NULL) == -1) {
		perror("Error: cannot handle SIGTERM");
	}

	// apply an action for SIGUSR1
	if (sigaction(SIGUSR1, &action, NULL) == -1) {
		perror("Error: cannot handle SIGUSR1");
	}

	// keep running
	while (1);
    
}

void handle_signal(int signal) {
	switch (signal) {
		// if the signal is sigterm exit successfully
		case SIGTERM:
			printf("\n");
			exit(0);
		break;

		// if the signal is SIGUSR1 print all the new characters
		case SIGUSR1:
			while (i < 100){
				if (map[i] == '\0')
					break;
				printf("%c", map[i]);
				i++;
			}
            	break;
	}
}

