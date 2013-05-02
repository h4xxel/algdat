#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

struct COUNTRY_NODE {
	char code[3];
	char *name;
	struct COUNTRY_NODE *next;
};

struct CONTINENT {
	char code[3];
	char *name;
	struct COUNTRY_NODE *countries;
	struct COUNTRY_NODE *last;
} continent[]={
	{"AF", "Africa"},
	{"AS", "Asia"},
	{"EU", "Europe"},
	{"NA", "North America"},
	{"SA", "South America"},
	{"OC", "Oceania"},
	{"AN", "Antarctica"},
};

int main(int argc, char **argv) {
	FILE *f;
	struct COUNTRY_NODE tmp, *country;
	char cont[2];
	char c;
	int i;
	
	if(argc==2) {
		if(!(f=fopen(argv[1], "r"))) {
			fprintf(stderr, "Unable to open file\n");
			return 1;
		}
	} else if(argc==1)
		f=stdin;
	else {
		fprintf(stderr, "Usage: uppg4 [countries text file]\n");
		return 1;
	}
	
	next:
	while(!feof(f)) {
		c=getc(f);
		if(c=='\n')
			continue;
		if(c=='#'||isspace(c)) {
			while(getc(f)!='\n');
			continue;
		}
		ungetc(c, f);
		fscanf(f, "%2c %s %*s %*s %m[^\n]", cont, tmp.code, &tmp.name);
		
		for(i=0; i<sizeof(continent)/sizeof(struct CONTINENT); i++) {
			if(*((short *) &cont)==*((short *) &continent[i].code)) {
				country=malloc(sizeof(struct COUNTRY_NODE));
				memcpy(country, &tmp, sizeof(struct COUNTRY_NODE));
				country->next=NULL;
				if(continent[i].last) {
					continent[i].last->next=country;
					continent[i].last=country;
				} else
					continent[i].countries=continent[i].last=country;
				
				goto next;
			}
		}
		fprintf(stderr, "Warning: found country with invalid continent\n");
		free(tmp.name);
	}
	
	printf("Countries by continent\n");
	for(i=0; i<sizeof(continent)/sizeof(struct CONTINENT); i++) {
		printf("\n%s:\n", continent[i].name);
		for(country=continent[i].countries; country; country=country->next)
			printf(" * %s [%c%c]\n", country->name, country->code[0], country->code[1]);
	}
	
	close(f);
	return 0;
}
