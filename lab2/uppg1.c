#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LIFO_SIZE 1024

struct NODE {
	unsigned int value;
	struct NODE *next;
} *lifo_slow;

struct {
	unsigned int *sp;
	unsigned int stack[LIFO_SIZE];
} lifo_fast={
	lifo_fast.stack,
};

void lifo_push_fast(unsigned int value) {
	*lifo_fast.sp=value;
	lifo_fast.sp++;
}

unsigned int lifo_pop_fast() {
	lifo_fast.sp--;
	return *lifo_fast.sp;
}

void lifo_push_slow(unsigned int value) {
	struct NODE **node;
	for(node=&lifo_slow; *node; node=&((*node)->next));
	*node=malloc(sizeof(struct NODE));
	(*node)->next=NULL;
	(*node)->value=value;
}

unsigned int lifo_pop_slow() {
	unsigned int value;
	struct NODE **node;
	for(node=&lifo_slow; *node; node=&((*node)->next)) {
		if((*node)->next)
			continue;
		value=(*node)->value;
		free(*node);
		*node=NULL;
		return value;
	}
}

int main(int argc, char **argv) {
	struct timespec time_start, time_end;
	int i;
	
	printf("Pusing %u items to fast stack\n", LIFO_SIZE);
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	for(i=0; i<LIFO_SIZE; i++) {
		lifo_push_fast(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	printf("\tTime: %li ns\n", 1000000000L*(time_end.tv_sec-time_start.tv_sec)+(time_end.tv_nsec-time_start.tv_nsec));
	
	printf("Pusing %u items to slow stack\n", LIFO_SIZE);
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	for(i=0; i<LIFO_SIZE; i++) {
		lifo_push_slow(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	printf("\tTime: %li ns\n", 1000000000L*(time_end.tv_sec-time_start.tv_sec)+(time_end.tv_nsec-time_start.tv_nsec));
	
	
	printf("Popping %u items from fast stack\n", LIFO_SIZE);
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	for(i=0; i<LIFO_SIZE; i++) {
		lifo_pop_fast(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	printf("\tTime: %li ns\n", 1000000000L*(time_end.tv_sec-time_start.tv_sec)+(time_end.tv_nsec-time_start.tv_nsec));
	
	printf("Popping %u items from slow stack\n", LIFO_SIZE);
	clock_gettime(CLOCK_MONOTONIC, &time_start);
	for(i=0; i<LIFO_SIZE; i++) {
		lifo_pop_slow(i);
	}
	clock_gettime(CLOCK_MONOTONIC, &time_end);
	printf("\tTime: %li ns\n", 1000000000L*(time_end.tv_sec-time_start.tv_sec)+(time_end.tv_nsec-time_start.tv_nsec));
	
	return 0;
}
