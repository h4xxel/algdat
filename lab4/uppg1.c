#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define COUNTRY_TABLE 1024

struct COUNTRY {
	char *name;
	char *capitol;
	unsigned long population;
	unsigned long area;
	
	unsigned int hash;
	struct COUNTRY *next;
} *country[COUNTRY_TABLE];

unsigned int hash_string(const char *string) {
	unsigned int hash=0;
	unsigned short i;
	for(i=1; *string; string++, hash+=i*tolower(*string), i*=31);
	return hash;
}

int db_insert(struct COUNTRY *newc) {
	unsigned int hash=hash_string(newc->name);
	struct COUNTRY **c;
	for(c=&country[hash%COUNTRY_TABLE]; *c; c=&(*c)->next) {
		if(!strcmp(newc->name, (*c)->name)) {
			return -1;
		}
	}
	newc->hash=hash;
	newc->next=*c;
	*c=newc;
	return 0;
}

void print_country(char *name) {
	unsigned int hash;
	struct COUNTRY *c;
	hash=hash_string(name);
	for(c=country[hash%COUNTRY_TABLE]; c; c=c->next) {
		if(strncasecmp(name, c->name, strlen(name)))
			continue;
		printf("%s\nCapitol: %s\nPopulation: %lu\nArea: %lu km²\n", c->name, c->capitol, c->population, c->area);
		return;
	}
	printf("Country '%s' not found in database.\n", name);
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
			if(db_insert(c)) {
				free(c->name);
				free(c->capitol);
				free(c);
			}
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
			print_country(search);
		free(search);
	} while(search);
	return 0;
}
