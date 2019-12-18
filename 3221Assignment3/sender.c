/* Assumption that make will be called before every use of sender and receiver */

#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>

void main(int argc, char *argv[]){
	
	// read the pid from input
	int pid;
	if (argc > 1)
		pid = atoi(argv[1]);

	// open the file
	int fd = open("sharedfile", O_RDWR, (mode_t)0600);
	if(fd == -1){
		fprintf(stderr, "error could not open file");
		exit(1);
	}
	lseek(fd, 99, SEEK_SET);

	// create a counter variable	
	int i = 0;
	// create a char pointer to the memory map
	char * map = mmap(0, 100,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);

	// while the amount of chacters read are less than 100
	// and there is still more input to read, 
	// read the next char and increment i
	while(i < 100 && scanf("%c", &map[i++]))
	{
		// sync the map and signal SIGUSR1
		msync(map, 100, MAP_SHARED);
		kill(pid, SIGUSR1);
	}

	// unmap the map pointer
	munmap(map, 100);
	// signal SIGTERM
	kill(pid, SIGTERM);
	// close the file
	close(fd);
}
