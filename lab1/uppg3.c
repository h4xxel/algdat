#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int binary_search_iterative(int *seq, int lbound, int ubound, int val) {
	int i;
	while(ubound>=lbound) {
		i=(lbound+ubound)/2;
		if(seq[i]==val)
			return i;
		else if(seq[i]>val)
			ubound=i-1;
		else
			lbound=i+1;
	}
	return -1;
}

int binary_search_recursive(int *seq, int lbound, int ubound, int val) {
	int i=(lbound+ubound)/2;
	if(ubound<lbound)
		return -1;
	if(seq[i]==val)
		return i;
	else if(seq[i]>val)
		return binary_search_recursive(seq, lbound, i-1, val);
	return binary_search_recursive(seq, i+1, ubound, val);
}

int sequential_search(int *seq, int lbound, int ubound, int val) {
	for(; lbound<ubound; lbound++)
		if(seq[lbound]==val)
			return lbound;
	return -1;
}

struct SEARCH_ALGORITHM {
	const char *name;
	int (*f)(int *, int, int, int);
} search_algorithm[]={
	{"iterative binary search", binary_search_iterative},
	{"recursive binary search", binary_search_recursive},
	{"sequential search", sequential_search},
	{NULL, NULL},
};

int main(int argc, char **argv) {
	struct SEARCH_ALGORITHM *search;
	struct timespec time_start, time_end;
	unsigned int ms;
	int *sequence;
	int size;
	int i, a;
	
	if(argc!=2) {
		fprintf(stderr, "Usage: uppg3 <number of items in sequence>\n");
		return 1;
	}
	size=atoi(argv[1]);
	if(size<=0) {
		fprintf(stderr, "Fatal: number of items must be a positive, non-zero integer\n");
		return 1;
	}
	
	srand(time(NULL));
	sequence=malloc(size*sizeof(int));
	printf("Sequence is: \n[ ");
	for(a=i=0; i<size; i++) {
		a=sequence[i]=a+3/*+rand()%5*/;
		if(i<100)
			printf("%i ", a);
	}
	if(i>=100)
		printf("... %i %i %i ", sequence[i-3], sequence[i-2], sequence[i-1]);
	printf("]\n");
	
	a=rand()%a;
	printf("Randomly selected value %i to search for\n", a);
	
	for(search=search_algorithm; search->f; search++) {
		printf("\nSearching using %s\n", search->name);
		clock_gettime(CLOCK_MONOTONIC, &time_start);
		i=search->f(sequence, 0, size, a);
		clock_gettime(CLOCK_MONOTONIC, &time_end);
		if(i>=0)
			printf("Value found at index %i\n", i);
		else
			printf("Value not found\n");
		printf("Time: %li ns\n", 1000000000L*(time_end.tv_sec-time_start.tv_sec)+(time_end.tv_nsec-time_start.tv_nsec));
	}
		
	free(sequence);
	return 0;
}