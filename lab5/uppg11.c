#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HEAP_PARENT(i) (((i)-1)/2)
#define HEAP_LEFT(i) (2*(i)+1)
#define HEAP_RIGHT(i) (2*(i)+2)

struct PRIORITY_QUEUE_ELEMENT {
	int priority;
	void *item;
	void *prev;
};

struct PRIORITY_QUEUE {
	struct PRIORITY_QUEUE_ELEMENT *array_heap;
	int elements;
	int size;
};

struct EDGE {
	int weight;
	struct VERTEX *vertex;
	struct EDGE *next;
};

struct VERTEX {
	char *name;
	struct EDGE *neighbour;
};

struct GRAPH {
	struct VERTEX *vertex;
	struct GRAPH *next;
};

struct VISITED {
	struct VERTEX *vertex;
	struct VERTEX *prev;
	struct VISITED *next;
};

enum DIRECTION {
	DIRECTION_SINGLE,
	DIRECTION_DOUBLE,
};

struct GRAPH *graph;

struct VERTEX *graph_add_vertex(struct GRAPH **graph, char *name) {
	struct GRAPH *g=*graph;
	*graph=malloc(sizeof(struct GRAPH));
	(*graph)->next=g;
	(*graph)->vertex=malloc(sizeof(struct VERTEX));
	(*graph)->vertex->name=name;
	(*graph)->vertex->neighbour=NULL;
	return (*graph)->vertex;
}

void graph_add_edge(struct VERTEX *v1, struct VERTEX *v2, int weight, enum DIRECTION direction) {
	struct EDGE *e;
	for(e=v1->neighbour; e; e=e->next)
		if(e->vertex==v2) {
			printf("Warning: %s already linked to %s\n", v1->name, v2->name);
			return;
		}
	e=v1->neighbour;
	v1->neighbour=malloc(sizeof(struct EDGE));
	v1->neighbour->next=e;
	v1->neighbour->vertex=v2;
	v1->neighbour->weight=weight;
	if(direction!=DIRECTION_DOUBLE)
		return;
	e=v2->neighbour;
	v2->neighbour=malloc(sizeof(struct EDGE));
	v2->neighbour->next=e;
	v2->neighbour->vertex=v1;
	v2->neighbour->weight=weight;
}

void priority_queue_add(struct PRIORITY_QUEUE *queue, void *item, void *prev, int priority) {
	int i, p;
	struct PRIORITY_QUEUE_ELEMENT tmp;
	if(queue->elements+1>queue->size) {
		queue->array_heap=realloc(queue->array_heap, (queue->size+100)*sizeof(struct PRIORITY_QUEUE_ELEMENT));
		queue->size+=100;
	}
	queue->array_heap[queue->elements].item=item;
	queue->array_heap[queue->elements].priority=priority;
	queue->array_heap[queue->elements].prev=prev;
	i=queue->elements++;
	for(p=HEAP_PARENT(i); i&&queue->array_heap[i].priority<queue->array_heap[p].priority; i=p, p=HEAP_PARENT(i)) {
		tmp=queue->array_heap[p];
		queue->array_heap[p]=queue->array_heap[i];
		queue->array_heap[i]=tmp;
	}
}

struct PRIORITY_QUEUE_ELEMENT priority_queue_pop(struct PRIORITY_QUEUE *queue) {
	int i, j, l, r;
	struct PRIORITY_QUEUE_ELEMENT element, tmp;
	element=queue->array_heap[0];
	queue->elements--;
	queue->array_heap[0]=queue->array_heap[queue->elements];
	i=0;
	do {
		j=-1;
		r=HEAP_RIGHT(i);
		if(r<queue->elements&&queue->array_heap[r].priority<queue->array_heap[i].priority) {
			l=HEAP_LEFT(i);
			if(queue->array_heap[l].priority<queue->array_heap[r].priority)
				j=l;
			else
				j=r;
		} else {
			l=HEAP_LEFT(i);
			if(l<queue->elements&&queue->array_heap[l].priority<queue->array_heap[i].priority)
				j=l;
		}
		if (j>=0) {
			tmp=queue->array_heap[j];
			queue->array_heap[j]=queue->array_heap[i];
			queue->array_heap[i]=tmp;
		}
		i=j;
	} while (i>=0);
	
	return element;
}

struct PRIORITY_QUEUE *priority_queue_create() {
	struct PRIORITY_QUEUE *queue;
	queue=malloc(sizeof(struct PRIORITY_QUEUE));
	queue->elements=0;
	queue->size=100;
	queue->array_heap=calloc(100, sizeof(struct PRIORITY_QUEUE_ELEMENT));
	return queue;
}

void graph_print_shortest_path(struct VERTEX *v1, struct VERTEX *v2) {
	struct EDGE *e;
	struct PRIORITY_QUEUE_ELEMENT element={0};
	struct PRIORITY_QUEUE *queue=priority_queue_create();
	struct VISITED *visited=NULL, *l;
	for(;;) {
		if(v1==v2) {
			for(l=visited; l; l=l->next) {
				if(l->vertex==v1) {
					printf("%s←", l->vertex->name);
					v1=l->prev;
				}
			}
			printf("%s\n", v1->name);
			return;
		}
		for(e=v1->neighbour; e; e=e->next) {
			for(l=visited; l; l=l->next)
				if(l->vertex==e->vertex)
					break;
			if(!l)
				priority_queue_add(queue, e->vertex, v1, e->weight+element.priority);
		}
		if(!queue->elements)
			break;
		element=priority_queue_pop(queue);
		v1=element.item;
		l=visited;
		visited=malloc(sizeof(struct VISITED));
		visited->vertex=v1;
		visited->next=l;
		visited->prev=element.prev;
	}
	printf("no path found\n");
}

int db_load() {
	FILE *f;
	char *line, *city;
	int distance;
	struct GRAPH *g;
	struct VERTEX *v1=NULL, *v2;
	if(!(f=fopen("eroads.csv", "r")))
		return 1;
	
	while(!feof(f)) {
		fscanf(f, "%m[^\n]", &line);
		getc(f);
		if(!line)
			continue;
		if(!strncasecmp(line, "Country", 7)) {
			v2=NULL;
			goto next;
		}
		strtok(line, ";");
		city=strtok(NULL, ";");
		for(g=graph; g; g=g->next)
			if(!strcasecmp(city, g->vertex->name))
				break;
		if(g)
			v2=g->vertex;
		else
			v2=graph_add_vertex(&graph, strdup(city));
		if(!v1)
			goto next;
		distance=atoi(strtok(NULL, ";"));
		graph_add_edge(v1, v2, distance, DIRECTION_DOUBLE);
		
		next:
		v1=v2;
		free(line);
	}
	
	fclose(f);
}

int main(int argc, char **argv) {
	char *city1, *city2;
	struct VERTEX *v1, *v2;
	struct GRAPH *g;
	db_load();
	
	printf("\nEurope roadmap, Axel Isaksson 2013\n\n");
	for(;;) {
		printf("city 1>");
		scanf("%m[^\n]", &city1);
		getchar();
		printf("city 2>");
		scanf("%m[^\n]", &city2);
		getchar();
		v1=v2=NULL;
		for(g=graph; g&&!(v1&&v2); g=g->next) {
			if(!strcasecmp(city1, g->vertex->name)) {
				v1=g->vertex;
			}
			if(!strcasecmp(city2, g->vertex->name)) {
				v2=g->vertex;
			}
		}
		if(g) {
			graph_print_shortest_path(v1, v2);
		} else {
			printf("city not found\n");
		}
		
		free(city1);
		free(city2);
	}
	
	return 0;
}
