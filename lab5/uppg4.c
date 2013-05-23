#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	struct VISITED *next;
};

struct VERTEX *graph_add_vertex(struct GRAPH **graph, char *name) {
	struct GRAPH *g=*graph;
	*graph=malloc(sizeof(struct GRAPH));
	(*graph)->next=g;
	(*graph)->vertex=malloc(sizeof(struct VERTEX));
	(*graph)->vertex->name=name;
	(*graph)->vertex->neighbour=NULL;
	return (*graph)->vertex;
}

void graph_add_edge(struct VERTEX *v1, struct VERTEX *v2, int weight) {
	struct EDGE *e;
	for(e=v1->neighbour; e; e=e->next)
		if(e->vertex==v2)
			return;
	e=v1->neighbour;
	v1->neighbour=malloc(sizeof(struct EDGE));
	v1->neighbour->next=e;
	v1->neighbour->vertex=v2;
	v1->neighbour->weight=weight;
	e=v2->neighbour;
	v2->neighbour=malloc(sizeof(struct EDGE));
	v2->neighbour->next=e;
	v2->neighbour->vertex=v1;
	v2->neighbour->weight=weight;
}

int graph_exists_path(struct VERTEX *v1, struct VERTEX *v2, struct VISITED **visited) {
	struct EDGE *e;
	struct VISITED *v;
	printf("	Visit %s\n", v1->name);
	if(v1==v2)
		return 1;
	for(v=*visited; v; v=v->next)
		if(v1==v->vertex) {
			printf("		%s already visited\n", v1->name);
			return 0;
		}
	v=malloc(sizeof(struct VISITED));
	v->next=*visited;
	v->vertex=v1;
	*visited=v;
	for(e=v1->neighbour; e; e=e->next) {
		if(graph_exists_path(e->vertex, v2, visited))
			return 1;
	}
	return 0;
}

void exists_path(struct VERTEX *v1, struct VERTEX *v2) {
	struct VISITED *visited=NULL, *next;
	printf("Looking for path between %s and %s\n", v1->name, v2->name);
	printf("	* There is %s path between %s and %s\n", graph_exists_path(v1, v2, &visited)?"a":"no", v1->name, v2->name);
	for(; visited; visited=next) {
		next=visited->next;
		free(visited);
	}
}

int main(int argc, char **argv) {
	struct GRAPH *g=NULL;
	struct VERTEX *stockholm, *uppsala, *gavle, *vasteras, *visby;
	stockholm=graph_add_vertex(&g, "stockholm");
	uppsala=graph_add_vertex(&g, "uppsala");
	gavle=graph_add_vertex(&g, "gävle");
	vasteras=graph_add_vertex(&g, "västerås");
	visby=graph_add_vertex(&g, "visby");
	
	graph_add_edge(stockholm, uppsala, 71);
	graph_add_edge(gavle, uppsala, 105);
	graph_add_edge(uppsala, vasteras, 79);
	graph_add_edge(stockholm, vasteras, 110);
	
	exists_path(stockholm, vasteras);
	exists_path(stockholm, gavle);
	exists_path(uppsala, visby);
	return 0;
}
