#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct COUNTRY {
	char *name;
	char *capitol;
	unsigned long population;
	unsigned long area;
	
	struct COUNTRY *left;
	struct COUNTRY *right;
} *country;

struct COUNTRY *avl_rotate_left(struct COUNTRY *a) {
	struct COUNTRY *b=a->right;
	a->right=b->left;
	b->left=a;
	return b;
}

struct COUNTRY *avl_rotate_right(struct COUNTRY *a) {
	struct COUNTRY *b=a->left;
	a->left=b->right;
	b->right=a;
	return b;
}

struct COUNTRY *db_insert(struct COUNTRY *node, struct COUNTRY *root) {
	int res;
	if(!root)
		return node;
	res=strcasecmp(root->name, node->name);
	if(!res) {
		free(node->name);
		free(node->capitol);
		free(node);
	} else if (res>0)
		root->left=db_insert(node, root->left);
	else
		root->right=db_insert(node, root->right);
	return root;
}

void print_country(char *name, struct COUNTRY *root) {
	int res;
	if(!root) {
		printf("Unable to find country '%s' in database.\n", name);
		return;
	}
	res=strcasecmp(root->name, name);
	if(!res) {
		printf("%s\nCapitol: %s\nPopulation: %lu\nArea: %lu km²\n", root->name, root->capitol, root->population, root->area);
	} else if(res>0) 
		print_country(name, root->left);
	else
		print_country(name, root->right);
}

void load_db() {
	FILE *f;
	char *s;
	struct COUNTRY *c;
	if(!(f=fopen("countries", "r"))) {
		puts("Warning: unable to load database");
		return;
	}
	c=calloc(sizeof(struct COUNTRY), 1);
	while(!feof(f)) {
		if(!fscanf(f, "%m[^\n]", &s)) {
			country=db_insert(c, country);
			c=calloc(sizeof(struct COUNTRY), 1);
			getc(f);
			continue;
		}
		getc(f);
		if(!s)
			continue;
		if(!*s) {
			free(s);
			continue;
		}
		sscanf(s, "Country: %m[^\n]", &c->name);
		sscanf(s, "Capitol: %m[^\n]", &c->capitol);
		sscanf(s, "Population: %lu", &c->population);
		sscanf(s, "Area: %lu", &c->area);
		free(s);
	}
	free(c);
	fclose(f);
}

int main(int argc, char **argv) {
	char *search;
	load_db();
	do {
		printf("search>");
		scanf("%m[^\n]", &search);
		getchar();
		if(search)
			print_country(search, country);
		free(search);
	} while(search);
	return 0;
}
