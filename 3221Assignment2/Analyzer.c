#include <stdio.h>
#include <stdlib.h>
#include <math.h>

FILE * fp;
int main()  {
	long input, last;
	int sum = 0, n = 0;
	double mean, stdev;

	// open the file for reading
	fp = fopen("pthread_stats", "r");
	if (fp == NULL)
		return 1;
	
	// for everyline in the file get the long value and set input to that value
	while (fscanf(fp,"%ld%*[^\n]", &input) != -1){
		// if the last input is not equal to this input add one to n
		if (last != input)
			n++;
		// add one to the current series of the same thread
		sum++;
		// set the last input to be the current input
		last = input;
	}
	// set the value of mean
	mean = (double)sum / (double)n;
	// close the file
	fclose(fp);
	// set last and input to two values that aren't equal
	last = -2;
	input = -1;
	sum = 0;
	// open the file again for reading
	fp = fopen("pthread_stats", "r");
	if (fp == NULL)
		return 1;
	// for every line in the file get the long value and set input to that value
	while (fscanf(fp,"%d%*[^\n]", &input) != -1){
		// if the last is not equal to the input (not on the first time on this loop
		if (last != input && last != -2){
			// stdev is added by the the (last thread's consecutive number - the mean)^2
			stdev += pow((double)sum - mean, 2);
			// move onto the next thread's consecutive numbers
			sum = 0;
		}
		// add one to the thread's consecutive number
		sum++;
		// set the last input to be the current input
		last = input;
	}
	// close the file
	fclose(fp);
	// calculate the standard deviation
	stdev += pow((double)sum - mean, 2);
	stdev = stdev / (double)n;
	stdev = sqrt(stdev);

	// print the mean and standard deviation, and exit
	printf("mean:			%lf\n", mean);
	printf("standard deviation:	%lf\n", stdev);

	return 0;
}
