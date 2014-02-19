/*
Author: David White
Program: Multi-threaded Merge Sort 
Date: 2/18/2014
About: This program creates a random double and inputs it inside the large array.
Then the program takes and splits the array and then splits it again and finally 
sorts the data and then merges the arrays back up the tree.

No known errors

USER NOTICE:
This program when ran will output the input data in the terminal as well as the sorted array

THE ACTUAL TIME THE COMPUTER SPENT SORTING WILL PRINT AT THE END.
IT WILL BE LESS THAN THE TOTAL PROGRAM PROCESS TIME - Due to print statements to demo to the teacher

command to compile:
gcc -pthread -std=c99 -o mergesort mergeThreads.c

// Demo of the Output 
Index: 999980 >> Data: 9.999836e-01
Index: 999981 >> Data: 9.999839e-01
Index: 999982 >> Data: 9.999844e-01
Index: 999983 >> Data: 9.999868e-01
Index: 999984 >> Data: 9.999908e-01
Index: 999985 >> Data: 9.999916e-01
Index: 999986 >> Data: 9.999921e-01
Index: 999987 >> Data: 9.999922e-01
Index: 999988 >> Data: 9.999929e-01
Index: 999989 >> Data: 9.999958e-01
Index: 999990 >> Data: 9.999960e-01
Index: 999991 >> Data: 9.999961e-01
Index: 999992 >> Data: 9.999966e-01
Index: 999993 >> Data: 9.999972e-01
Index: 999994 >> Data: 9.999983e-01
Index: 999995 >> Data: 9.999986e-01
Index: 999996 >> Data: 9.999992e-01
Index: 999997 >> Data: 9.999994e-01
Index: 999998 >> Data: 9.999997e-01
Index: 999999 >> Data: 9.999998e-01
Total Sort Time: 0.503133 sec.
david@ubuntu:~/OS$

*/
#include <pthread.h> // needed for threads
#include <stdio.h>
#include <stdlib.h>
#include <time.h>  // needed to get the time
#include <sys/time.h> // to get the time
#include <string.h> // to get strings

#define memSize 1000000
#define THREAD_LEVEL 3

void threadsMergeSort(double arrgument[], int size, int tlevel);
void *threadsMergeSortSetup(void *threadarg);
struct threadControlData{
	double *arrgument;
	double *back;
	int low;
	int high;
	int level;
};
int compare (const void *a, const void *b);

int main (int argc, char *argv[])
{
	struct timeval start, end;
	double outputTime;

	srand(time(NULL)); 
	int memory = memSize;
	if (argc == 2) 
	{
		memory = atoi(argv[1]);
	}

	double *arrgumentbck = (double *) malloc(memory*sizeof(double));
	double *arrgument = (double *) malloc(memory*sizeof(double));
	printf("Please Wait >> Threads Working\n");
	for(int i = 0; i < memory; i ++)
	{
		arrgumentbck[i] = 1.0*rand()/RAND_MAX;

		//printf("%e", arrgument[i]);
	}
	printf("Threads will sleep for a few seconds to demo them working >>\n");
	sleep(10);
	memcpy(arrgument, arrgumentbck, memory*sizeof(double));

	gettimeofday(&start, NULL);
	threadsMergeSort(arrgument, memory, THREAD_LEVEL);
	gettimeofday(&end, NULL);

	if (!gotSortedPrintArray(arrgument, memory))
	{
		printf("ERROR >> Arrgument FAILED to Sort.\n");
	}
	
	//Compute time 
	outputTime = ((end.tv_sec * 1000000 + end.tv_usec)
		  - (start.tv_sec * 1000000 + start.tv_usec))/1000000.0;
	printf("Total Sort Time: %lf sec.\n", outputTime);
	
	free(arrgument);
	free(arrgumentbck);
	pthread_exit(NULL);
}

void mergesort(double arrgument[], int size)
{
	double *back = (double *) malloc(size*sizeof(double));
	mergeSortWrapper(arrgument, back, 0, size-1);
	free(back);
}

void mergeSortWrapper(double arrgument[], double back[], int low, int high)
{
	if (low == high) return;
	int mid = (low + high)/2;
	mergeSortWrapper(arrgument, back, low, mid);
	mergeSortWrapper(arrgument, back, mid+1, high);
	merge(arrgument, back, low, mid, high);
}

void merge(double arrgument[], double back[], int low, int mid, int high)
{
	int ai = low, bi = mid + 1, i = low;
	while (ai <= mid && bi <= high)
	{
		if (arrgument[ai] <= arrgument[bi])
		{
			back[i] = arrgument[ai];
			ai ++;
		}else
		{
			back[i] = arrgument[bi];
			bi ++;
		}
		i ++;
	}
	if (ai > mid)
	{
		memcpy(&back[i], &arrgument[bi], (high-bi+1)*sizeof(double));
	}else
	{
		memcpy(&back[i], &arrgument[ai], (mid-ai+1)*sizeof(double));
	}
	memcpy(&arrgument[low], &back[low], (high-low+1)*sizeof(double));
}

int gotSortedPrintArray(double arrgument[], int size)
{
	for (int i = 1; i < size; i ++)
	{
		printf("Index: %d >> Data: %e\n",i, arrgument[i]); //  THIS WILL PRINT OUT THE SORTED LIST IT TAKES A WHILE
		if (arrgument[i] < arrgument[i-1]) 
		{
			printf("%e < %e\n", arrgument[i], arrgument[i-1]);
			return 0;
		}
	}
	return 1;
}

void threadsMergeSort(double arrgument[], int size, int tlevel)
{
	int returnCode;
	void *status;

	double *back = (double *) malloc(size*sizeof(double));

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	struct threadControlData td;
	td.arrgument = arrgument;
	td.back = back;
	td.low = 0;
	td.high = size - 1;
	td.level = tlevel;

	//The top-level thread.
	pthread_t theThread;
	returnCode = pthread_create(&theThread, &attr, threadsMergeSortSetup,
						(void *) &td);
	if (returnCode) 
	{
    	printf("ERROR; pthread_join() >> fail >> is %d\n", returnCode);
    	exit(-1);
    }
	pthread_attr_destroy(&attr);
	returnCode = pthread_join(theThread, &status);
	if (returnCode)
	{
		printf("ERROR; pthread_join() >> fail >> is %d\n", returnCode);
		exit(-1);
	}
	free(back);

}
int compare(const void *a, const void *b)
{
	const double *da = (const double *)a;
	const double *db = (const double *)b;

	return (*da > *db) - (*da < *db);
}


void *threadsMergeSortSetup(void *threadarg)
{
	int mid, t, returnCode;
	void *status;

	struct threadControlData *my_data;
	my_data = (struct threadControlData *) threadarg;
	
	if (my_data->level <= 0 || my_data->low == my_data->high)
	{
		
		mergeSortWrapper(my_data->arrgument, my_data->back, 
						my_data->low, my_data->high);
		pthread_exit(NULL);
	}
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

	struct threadControlData threadControlData_array[2];
	mid = (my_data->low + my_data->high)/2;

	for (t = 0; t < 2; t ++)
	{
		threadControlData_array[t].arrgument = my_data->arrgument;
		threadControlData_array[t].back = my_data->back;
		threadControlData_array[t].level = my_data->level - 1;
	}
	threadControlData_array[0].low = my_data->low;
	threadControlData_array[0].high = mid;
	threadControlData_array[1].low = mid+1;
	threadControlData_array[1].high = my_data->high;

//start the threads.
	pthread_t threads[2];
	for (t = 0; t < 2; t ++)
	{
		returnCode = pthread_create(&threads[t], &attr, threadsMergeSortSetup,
							(void *) &threadControlData_array[t]);
		if (returnCode) 
		{
    		printf("ERROR; pthread_join() >> fail >> is %d\n", returnCode);
    		exit(-1);
    	}
	}
	
	pthread_attr_destroy(&attr);
//left and right threads and merge.
	for (t = 0; t < 2; t ++)
	{
		returnCode = pthread_join(threads[t], &status);
		if (returnCode)
		{
			printf("ERROR; pthread_join() >> fail >> is %d\n", returnCode);
			exit(-1);
		}
	}
	merge(my_data->arrgument, my_data->back, my_data->low, mid, my_data->high);
	
	pthread_exit(NULL);
}





