#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define ARRAY_SIZE 64

enum DIRECTION {
	DIRECTION_SINGLE,
	DIRECTION_DOUBLE,
};

const char *menu=\
	"graph\n"
	"1. add vertex\n"
	"2. remove vertex\n"
	"3. add edge\n"
	"4. remove edge\n"
	"5. list graph\n"
	"6. quit\n";

int graph[ARRAY_SIZE][ARRAY_SIZE];
char *vertex[ARRAY_SIZE];

int graph_add_vertex(char *value) {
	int i;
	for(i=0; i<ARRAY_SIZE; i++)
		if(!vertex[i])
			break;
	vertex[i]=value;
	return i;
}

void graph_remove_vertex(int index) {
	free(vertex[index]);
	vertex[index]=NULL;
}

void graph_add_edge(int v1, int v2, int weight, enum DIRECTION direction) {
	graph[v1][v2]=weight;
	if(direction==DIRECTION_DOUBLE)
		graph[v2][v1]=weight;
}

void graph_remove_edge(int v1, int v2, enum DIRECTION direction) {
	graph[v1][v2]=0;
	if(direction==DIRECTION_DOUBLE)
		graph[v2][v1]=0;
}

void add_vertex(char *s1, char *s2) {
	graph_add_vertex(s1);
}

void remove_vertex(char *s1, char *s2) {
	int i;
	for(i=0; i<ARRAY_SIZE; i++) {
		if(!vertex[i])
			continue;
		if(!strcmp(s1, vertex[i])) {
			graph_remove_vertex(i);
			return;
		}
	}
}

void add_edge(char *s1, char *s2) {
	int i, v1=-1, v2=-1;
	for(i=0; i<ARRAY_SIZE; i++) {
		if(!vertex[i])
			continue;
		if(!strcmp(s2, vertex[i]))
			v1=i;
		if(!strcmp(s1, vertex[i]))
			v2=i;
	}
	if(!(v1==-1||v2==-1)) {
		graph_add_edge(v1, v2, 10, DIRECTION_SINGLE);
	}
}

void remove_edge(char *s1, char *s2) {
	int i, v1=-1, v2=-1;
	for(i=0; i<ARRAY_SIZE; i++) {
		if(!vertex[i])
			continue;
		if(!strcmp(s2, vertex[i]))
			v1=i;
		if(!strcmp(s1, vertex[i]))
			v2=i;
	}
	if(!(v1==-1||v2==-1)) {
		graph_remove_edge(v1, v2, DIRECTION_SINGLE);
	}
}

void print_graph() {
	int i, j;
	for(i=0; i<ARRAY_SIZE; i++) {
		if(!vertex[i])
			continue;
		printf("\n%s: ", vertex[i]);
		for(j=0; j<ARRAY_SIZE; j++)
			if(graph[i][j]&&vertex[j])
				printf("neighbours %s ", vertex[j]);
	}
	printf("\n");
}

int main(int argc, char **argv) {
	struct termios ttystate, ttysave;
	char *s1, *s2, c;
	void (*action[])(char *s1, char *s2)={
		add_vertex, remove_vertex, add_edge, remove_edge, print_graph,
	};
	srand(time(NULL));
	tcgetattr(STDIN_FILENO, &ttystate);
	ttysave=ttystate;
	ttystate.c_lflag&=~(ICANON);
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	
	puts(menu);
	for(;;) {
		putchar('>');
		c=getchar();
		putchar('\n');
		switch(c) {
			case '4':
			case '3':
				printf("first vertex:\nvertex>");
				scanf("%m[^\n]", &s2);
				getchar();
				printf("second vertex:\n");
				if(!s2)
					continue;
			case '1':
			case '2':
				printf("vertex>");
				scanf("%m[^\n]", &s1);
				getchar();
				if(!s1)
					continue;
			case '5':
				action[c-'1'](s1, s2);
				break;
			case '\n':
				puts(menu);
				break;
			case '6':
			case 'q':
			case 'Q':
				goto quit;
			default:
				puts("?");
		}
	}
	quit:
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
}
