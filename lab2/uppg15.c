#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define TIME_PER_PAGE 10
#define PAGE_PENALTY 3

struct QUEUE_NODE {
	void *object;
	int prio;
	struct QUEUE_NODE *prev;
	struct QUEUE_NODE *next;
};

struct PRINTER_JOB {
	unsigned int id;
	unsigned int pages;
	unsigned int computer;
	time_t time_remaining;
	/*more data here*/
};

enum PRINTER_STATUS {
	PRINTER_STATUS_QUEUE_EMPTY=-1,
	PRINTER_STATUS_OK,
};

struct {
	struct QUEUE_NODE *first;
	struct QUEUE_NODE *last;
	struct PRINTER_JOB current_job;
} printer_queue;

void printer_queue_push(struct PRINTER_JOB *job) {
	struct QUEUE_NODE *node, *traverse;
	struct PRINTER_JOB *j;
	int slip=PAGE_PENALTY;
	j=malloc(sizeof(struct PRINTER_JOB));
	node=malloc(sizeof(struct QUEUE_NODE));
	
	memcpy(j, job, sizeof(struct PRINTER_JOB));
	node->object=j;
	node->next=NULL;
	node->prev=NULL;
	node->prio=job->pages*PAGE_PENALTY*TIME_PER_PAGE;
	
	for(traverse=printer_queue.last; traverse; traverse=traverse->prev) {
		if(traverse->prio<=node->prio+15) {
			if(!(node->next=traverse->next))
				printer_queue.last=node;
			traverse->next=node;
			node->prev=traverse;
			return;
		} else {
			traverse->prio-=(traverse->prio-node->prio)*TIME_PER_PAGE/5;
			node->prio+=slip;
			slip+=PAGE_PENALTY;
			printf(" ->job %u slipped before job %u in queue\n", job->id, ((struct PRINTER_JOB *) traverse->object)->id);
		}
	}
	if(printer_queue.first)
		node->next=printer_queue.first;
	else
		printer_queue.last=node;
	printer_queue.first=node;
}

int printer_queue_pop(struct PRINTER_JOB *job) {
	struct QUEUE_NODE *node;
	if(printer_queue.first) {
		memcpy(job, printer_queue.first->object, sizeof(struct PRINTER_JOB));
		node=printer_queue.first;
		if(printer_queue.first->next) {
			printer_queue.first=printer_queue.first->next;
		} else {
			printer_queue.last=printer_queue.first=NULL;
		}
		free(node->object);
		free(node);
		return PRINTER_STATUS_OK;
	}
	return PRINTER_STATUS_QUEUE_EMPTY;
}

unsigned int printer_make_job(unsigned int computer, unsigned int pages) {
	struct PRINTER_JOB job;
	static unsigned int id=0;
	job.computer=computer;
	job.pages=pages;
	job.time_remaining=pages*TIME_PER_PAGE;
	job.id=++id;
	printer_queue_push(&job);
	return id;
}

void printer_do_work() {
	static char progress[]={
		'-',
		'\\',
		'|',
		'/',
	};
	static int progress_counter;
	struct PRINTER_JOB job;
	usleep(50000);
	if(printer_queue.current_job.time_remaining) {
		printer_queue.current_job.time_remaining--;
		fprintf(
			stderr, "%c Page %u/%u of job %u     \r",
			progress[progress_counter=++progress_counter%sizeof(progress)],
			((unsigned int) printer_queue.current_job.time_remaining)/TIME_PER_PAGE+1,
			printer_queue.current_job.pages,
			printer_queue.current_job.id
		);
		if(!(rand()%280)) {
			fprintf(stderr, "\x1b[31m * * * PAPER JAM, PRESS ENTER TO RESTORE * * *\x1b[0m\n");
			while(getchar()!='\n')
				usleep(50000);
		}
	} else {
		if(printer_queue.current_job.id) {
			printf("***Printer done with job %u\n", printer_queue.current_job.id);
			printer_queue.current_job.id=0;
		}
		if(printer_queue_pop(&printer_queue.current_job)==PRINTER_STATUS_OK)
			printf("***Printer started printing job %u\n", printer_queue.current_job.id);
	}
}

int main(int argc, char **argv) {
	struct termios ttystate, ttysave;
	char c;
	srand(time(NULL));
	tcgetattr(STDIN_FILENO, &ttystate);
	ttysave=ttystate;
	ttystate.c_lflag&=~(ICANON|ECHO);
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	
	printf("Starting printer spool server\n");
	for(;;) {
		c=getchar();
			
		if(c==27)
			break;
		else if(c>='0'&&c<='9') {
			unsigned int pages=(rand()%20)+1;
			printf("Computer %hhu added priner job with id %u (%u pages)\n", c-'0', printer_make_job(c-'0', pages), pages);
		}
		
		printer_do_work();
	}
	
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
	return 0;
}
