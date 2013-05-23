#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <darnit/darnit.h>

#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

struct POINT {
	int x;
	int y;
};

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
	struct POINT point;
	struct EDGE *neighbour;
};

struct GRAPH {
	struct VERTEX *vertex;
	struct GRAPH *next;
};

struct EDGE_LIST {
	int weight;
	struct VERTEX *v1;
	struct VERTEX *v2;
	struct EDGE_LIST *next;
};

enum DIRECTION {
	DIRECTION_SINGLE,
	DIRECTION_DOUBLE,
};

struct GRAPH *graph;
DARNIT_CIRCLE *circle[256];
DARNIT_LINE *line[256];
int length[256];
int vertices;
int edges;


struct VERTEX *graph_add_vertex(struct GRAPH **graph, struct POINT point) {
	struct GRAPH *g=*graph;
	*graph=malloc(sizeof(struct GRAPH));
	(*graph)->next=g;
	(*graph)->vertex=malloc(sizeof(struct VERTEX));
	(*graph)->vertex->point=point;
	(*graph)->vertex->neighbour=NULL;
	return (*graph)->vertex;
}

void graph_add_edge(struct VERTEX *v1, struct VERTEX *v2, int weight, enum DIRECTION direction) {
	struct EDGE *e;
	for(e=v1->neighbour; e; e=e->next)
		if(e->vertex==v2) {
			//printf("Warning: %s already linked to %s\n", v1->name, v2->name);
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

struct GRAPH *graph_prim(struct GRAPH *g) {
	struct EDGE *e;
	struct GRAPH *tree=NULL;
	struct EDGE_LIST *el=NULL, **tmp, **insert, *ne;
	struct VERTEX *v1, *v2;
	int a;
	if(!g)
		return;
	graph_add_vertex(&tree, g->vertex->point);
	for(; g; g=g->next)
		for(e=g->vertex->neighbour; e; e=e->next) {
			for(tmp=&el; *tmp; tmp=&((*tmp)->next))
				if((*tmp)->weight>e->weight)
					break;
			ne=malloc(sizeof(struct EDGE_LIST));
			ne->next=*tmp;
			ne->weight=e->weight;
			ne->v1=g->vertex;
			ne->v2=e->vertex;
			*tmp=ne;
		}
	do {
		for(tmp=&el; *tmp; tmp=&(*tmp)->next) {
			ne=*tmp;
			for(g=tree, a=0; g; g=g->next) {
				if(g->vertex->point.x==ne->v1->point.x&&g->vertex->point.y==ne->v1->point.y) {
					v1=g->vertex;
					a++;
				}
				if(g->vertex->point.x==ne->v2->point.x&&g->vertex->point.y==ne->v2->point.y) {
					v2=g->vertex;
					a+=2;
				}
			}
			if(!a)
				continue;
			else if(a==1)
				graph_add_edge(v1, graph_add_vertex(&tree, ne->v2->point), ne->weight, DIRECTION_SINGLE);
			else if(a==2)
				graph_add_edge(v2, graph_add_vertex(&tree, ne->v1->point), ne->weight, DIRECTION_SINGLE);
			
			*tmp=ne->next;
			free(ne);
			break;
		}
	} while(el);
	return tree;
}

void draw_graph(struct GRAPH *g) {
	int i, j;
	int a, b;
	struct EDGE *e;
	vertices=edges=0;
	if(!g)
		return;
	for(i=0, j=0; g; g=g->next, i++) {
		d_render_circle_move(circle[i], g->vertex->point.x, g->vertex->point.y, 10);
		for(e=g->vertex->neighbour; e; e=e->next, j++) {
			d_render_line_move(line[j], 0, g->vertex->point.x, g->vertex->point.y, e->vertex->point.x, e->vertex->point.y);
			a=MAX(g->vertex->point.x, e->vertex->point.x)-MIN(g->vertex->point.x, e->vertex->point.x);
			b=MAX(g->vertex->point.y, e->vertex->point.y)-MIN(g->vertex->point.y, e->vertex->point.y);
			length[i]=(a+b)/4;
			printf("length %i\n", length[i]);
		}
	}
	vertices=i;
	edges=j;
}

int main(int argc, char **argv) {
	int i, j;
	struct GRAPH *g, *tree=NULL;
	struct VERTEX *v;
	struct POINT p;
	DARNIT_KEYS keys;
	srand(time(NULL));
	d_init("Graphs", "uppg8", NULL);
	
	for(i=0; i<256; i++) {
		circle[i]=d_render_circle_new(32, 1);
		line[i]=d_render_line_new(1, 1);
	}
	
	for(j=0; j<4; j++)
		for(i=0; i<7; i++) {
			p.x=i*100+50;//(rand()%80)*10;
			p.y=j*100+50;//(rand()%48)*10;
			v=graph_add_vertex(&graph, p);
			for(g=graph; g; g=g->next)
				if(!(rand()%6))
					graph_add_edge(v, g->vertex, rand()%100, DIRECTION_DOUBLE);
		}
	tree=graph_prim(graph);
	draw_graph(graph);
	for(g=graph;;) {
		keys=d_keys_get();
		if(keys.start) {
			g=g==graph?tree:graph;
			draw_graph(g);
		}
		d_keys_set(keys);
		d_render_begin();
		d_render_tint(0xFF, 0xFF, 0xFF, 0xFF);
		for(i=0; i<vertices; i++)
			d_render_circle_draw(circle[i]);
		for(i=0; i<edges; i++) {
			d_render_tint(length[i], 0x7F, 0x0, 0xFF);
			d_render_line_draw(line[i], 1);
		}
		
		d_render_end();
		d_loop();
	}
	
	d_quit();
	return 0;
}
