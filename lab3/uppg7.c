#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

static void main_menu();
static void add_country();
static void remove_country();
static void edit_country_search();
static void edit_country();
static void show_country();
static void list_countries();
static void edit_neighbors();

static void db_save();
static void db_load();

struct NEIGHBOR {
	struct COUNTRY *country;
	char *name;
	struct NEIGHBOR *next;
};

struct COUNTRY {
	char *name;
	char *capitol;
	unsigned long population;
	unsigned long area;
	
	struct NEIGHBOR *neighbor;
	struct COUNTRY *next;
} *country, *country_edit;

static const char *help=
	"Countries v0.1\n"
	"Axel Isaksson 2013\n\n"
	"Global key actions\n"
	"	h. Show help\n"
	"	s. Save database\n"
	"	q. Quit application\n"
	"Pressing <enter> will show you the list of options\n"
	"in the current menu.\n"
	"Leaving a text entry blank will cancel the operation.\n";

struct {
	const char *text;
	void (*handler)();
} states[]={
	{
		"Main menu\n\n"
		"	1. Add country\n"
		"	2. Remove country\n"
		"	3. Edit country\n"
		"	4. Show country\n"
		"	5. List countries\n"
		"	6. Quit\n"
		"\nMake your selection",
		main_menu,
	}, {
		"Add country\n\n"
		"Enter a country name",
		add_country,
	}, {
		"Remove country\n\n"
		"Enter a country name",
		remove_country,
	}, {
		"Edit country\n\n"
		"Enter a country name",
		edit_country_search,
	}, {
		"Show country\n\n"
		"Enter a country name",
		show_country,
	}, {
		"List countries\n\n"
		"	1. Sorted by name\n"
		"	2. Sorted by population\n"
		"	3. Sorted by area\n"
		"	4. Main menu\n"
		"\nMake your selection",
		list_countries,
	}, {
		"Edit country\n\n"
		"	1. Change name\n"
		"	2. Change capitol\n"
		"	3. Change population size\n"
		"	4. Change area size\n"
		"	5. Edit neighbors\n"
		"	6. Done\n"
		"\nMake your selection",
		edit_country,
	}, {
		"Edit neighbors\n\n"
		"	1. Add neighbor\n"
		"	2. Remove neighbor\n"
		"	3. List neighbors\n"
		"	4. Done\n"
		"\nMake your selection",
		edit_neighbors,
	},
};

enum STATE {
	STATE_MAIN_MENU,
	STATE_ADD_COUNTRY,
	STATE_REMOVE_COUNTRY,
	STATE_EDIT_COUNTRY_SEARCH,
	STATE_SHOW_COUNTRY,
	STATE_LIST_COUNTRIES,
	STATE_EDIT_COUNTRY,
	STATE_EDIT_NEIGHBORS,
	
	STATES,
} state=STATE_MAIN_MENU, prevstate;

void switch_state(enum STATE s) {
	prevstate=state;
	state=s;
	puts(states[state].text);
}

static int compare_name(struct COUNTRY *c1, struct COUNTRY *c2) {
	return strcmp(c1->name, c2->name);
}

static int compare_capitol(struct COUNTRY *c1, struct COUNTRY *c2) {
	return strcmp(c1->capitol, c2->capitol);
}

static int compare_population(struct COUNTRY *c1, struct COUNTRY *c2) {
	if(c1->population==c2->population)
		return 0;
	if(c1->population>c2->population)
		return 1;
	return -1;
}

static int compare_area(struct COUNTRY *c1, struct COUNTRY *c2) {
	if(c1->area==c2->area)
		return 0;
	if(c1->area>c2->area)
		return 1;
	return -1;
}

static void menu_common(char input) {
	switch(input) {
		case '?':
		case 'h':
		case 'H':
			puts(help);
			break;
		case '\n':
			puts(states[state].text);
			break;
		case 's':
		case 'S':
			db_save();
			break;
		case 'm':
		case 'M':
			switch_state(STATE_MAIN_MENU);
			break;
		case 'q':
		case 'Q':
			db_save();
			exit(0);
			break;
		default:
			puts("?");
	}
}

static void main_menu() {
	char input=getchar();
	putchar('\n');
	switch(input) {
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			switch_state(input-'0');
			break;
		case '6':
			db_save();
			exit(0);
			break;
		default:
			menu_common(input);
	}
}

static void add_country() {
	struct COUNTRY *c;
	char *input;
	scanf("%m[^\n]", &input);
	
	if(!input) {
		getchar();
		switch_state(STATE_MAIN_MENU);
		return;
	}
	for(c=country; c; c=c->next)
		if(!strcasecmp(input, c->name)) {
			getchar();
			puts("Country already exists in database");
			puts(states[state].text);
			free(input);
			return;
		}
	c=malloc(sizeof(struct COUNTRY));
	c->next=country;
	c->neighbor=NULL;
	c->name=input;
	do {
		getchar();
		puts("Enter a capitol name");
		putchar('>');
		scanf("%m[^\n]", &c->capitol);
	} while(!c->capitol);
	
	do {
		getchar();
		puts("Enter population size");
		putchar('>');
	} while(!scanf("%lu", &c->population));
	
	do {
		getchar();
		puts("Enter area size");
		putchar('>');
	} while(!scanf("%lu", &c->area));
	getchar();
	
	country=c;
	printf("Added country %s\n", c->name);
	puts(states[state].text);
}

static void remove_country() {
	char *input;
	struct COUNTRY **cc, *c;
	struct NEIGHBOR *n, **nn;
	void *next, *p;
	scanf("%m[^\n]", &input);
	getchar();
	
	if(!input) {
		switch_state(STATE_MAIN_MENU);
		return;
	}
	for(cc=&country; *cc; cc=&((*cc)->next)) {
		if(strcasecmp((*cc)->name, input))
			continue;
		c=*cc;
		*cc=c->next;
		printf("Removed country %s\n", c->name);
		for(n=c->neighbor; n; n=next) {
			if(n->country)
				for(nn=&n->country->neighbor; *nn; nn=&((*nn)->next)) {
					if((*nn)->country==c) {
						p=*nn;
						free((*nn)->name);
						*nn=(*nn)->next;
						free(p);
						break;
					}
				}
			next=n->next;
			free(n->name);
			free(n);
		}
		free(c);
		break;
	}
	
	free(input);
	puts("Enter a country name");
}

static void edit_country_search() {
	char *input;
	struct COUNTRY *c;
	scanf("%m[^\n]", &input);
	getchar();
	
	if(!input) {
		switch_state(STATE_MAIN_MENU);
		return;
	}
	for(c=country; c; c=c->next) {
		if(strcasecmp(c->name, input))
			continue;
		printf("%s\n\tCapitol: %s\n\tPopulation: %lu\n\tArea: %lu km²\n\n", c->name, c->capitol, c->population, c->area);
		country_edit=c;
		switch_state(STATE_EDIT_COUNTRY);
		free(input);
		return;
	}
	puts("No country found by that name\n");
	
	free(input);
	puts("Enter a country name");
}

static void edit_country() {
	char input=getchar();
	char *text_input;
	static const char *alt[]={
		"Change name\nEnter a new name",
		"Change capitol\nEnter a new name",
		"Change population size\nEnter a new number",
		"Change area size\nEnter a new number",
	};
	void *p;
	enum {
		MOD_STRING,
		MOD_LONG,
	} mod;
	putchar('\n');
	switch(input) {
		case '1':
			p=&country_edit->name;
			mod=MOD_STRING;
			break;
		case '2':
			p=&country_edit->capitol;
			mod=MOD_STRING;
			break;
		case '3':
			p=&country_edit->population;
			mod=MOD_LONG;
			break;
		case '4':
			p=&country_edit->area;
			mod=MOD_LONG;
			break;
		case '5':
			switch_state(STATE_EDIT_NEIGHBORS);
			return;
		case '6':
			switch_state(STATE_EDIT_COUNTRY_SEARCH);
			return;
		default:
			menu_common(input);
			return;
	}
	puts(alt[input-'1']);
	putchar('>');
	scanf("%m[^\n]", &text_input);
	getchar();
	switch(mod) {
		case MOD_STRING:
			free(*((char **) p));
			*((char **) p)=text_input;
			break;
		case MOD_LONG:
			sscanf(text_input, "%lu", (unsigned long *) p);
		default:
			free(text_input);
	}
	puts(states[state].text);
}

static void show_country() {
	char *input;
	struct COUNTRY *c;
	struct NEIGHBOR *n;
	scanf("%m[^\n]", &input);
	getchar();
	
	if(!input) {
		switch_state(STATE_MAIN_MENU);
		return;
	}
	for(c=country; c; c=c->next) {
		if(strcasecmp(c->name, input))
			continue;
		printf("%s\n\tCapitol: %s\n\tPopulation: %lu\n\tArea: %lu km²\n", c->name, c->capitol, c->population, c->area);
		for(n=c->neighbor; n; n=n->next)
			if(n->country)
				printf("\tNeighbor: %s\n", n->country->name);
			else
				printf("\tOrphaned neighbor: %s\n", n->name);
		putchar('\n');
		break;
		
	}
	if(!c)
		puts("No country found by that name");
	
	free(input);
	puts("Enter a country name\n");
}

static void list_countries() {
	struct COUNTRY *sorted=NULL, *c, **cc;
	void *next;
	char input=getchar();
	static int tab_width=32;
	int tab_spaces;
	putchar('\n');
	static int (*compare[])(struct COUNTRY *c1, struct COUNTRY *c2)={
		compare_name,
		compare_capitol,
		compare_population,
		compare_area,
	};
	switch(input) {
		case '1':
		case '2':
		case '3':
			for(c=country; c; c=c->next) {
				next=NULL;
				for(cc=&sorted; *cc; cc=&((*cc)->next)) {
					if(compare[input-'0'](c, *cc)<0) {
						next=(*cc);
						break;
					}
				}
				*cc=malloc(sizeof(struct COUNTRY));
				memcpy(*cc, c, sizeof(struct COUNTRY));
				(*cc)->next=next;
			}
			for(tab_spaces=tab_width-printf("Country"); tab_spaces; tab_spaces--)
				putchar(' ');
			for(tab_spaces=tab_width-printf("Capitol"); tab_spaces; tab_spaces--)
				putchar(' ');
			for(tab_spaces=tab_width-printf("Population"); tab_spaces; tab_spaces--)
				putchar(' ');
			for(tab_spaces=tab_width-printf("Area"); tab_spaces; tab_spaces--)
				putchar(' ');
			putchar('\n');
			putchar('\n');
			for(c=sorted; c; c=next) {
				next=c->next;
				for(tab_spaces=tab_width-printf("%s", c->name); tab_spaces; tab_spaces--)
					putchar(' ');
				for(tab_spaces=tab_width-printf("%s", c->capitol); tab_spaces; tab_spaces--)
					putchar(' ');
				for(tab_spaces=tab_width-printf("%lu", c->population); tab_spaces; tab_spaces--)
					putchar(' ');
				for(tab_spaces=tab_width-printf("%lu", c->area); tab_spaces; tab_spaces--)
					putchar(' ');
				putchar('\n');
				free(c);
			}
			putchar('\n');
			break;
		case '4':
			switch_state(STATE_MAIN_MENU);
			break;
		default:
			menu_common(input);
	}
}

static void edit_neighbors() {
	char input=getchar();
	char *text_input;
	struct NEIGHBOR *n, **nn;
	struct COUNTRY *c;
	void *p;
	putchar('\n');
	switch(input) {
		case '1':
			add_try_again:
			puts("Add neighbor\nEnter a name");
			putchar('>');
			scanf("%m[^\n]", &text_input);
			getchar();
			if(!text_input) {
				puts(states[state].text);
				break;
			}
			for(nn=&country_edit->neighbor; *nn; nn=&((*nn)->next))
				if((*nn)->country) {
					if(!strcasecmp(text_input, (*nn)->country->name)) {
						printf("%s is already a neighbor to %s\n", (*nn)->country->name, country_edit->name);
						free(text_input);
						return;
					}
				} else
					if(!strcasecmp(text_input, (*nn)->name)) {
						printf("%s is already a neighbor to %s\n", (*nn)->country->name, country_edit->name);
						free(text_input);
						return;
					}
			for(c=country; c; c=c->next)
				if(!strcasecmp(text_input, c->name))
					break;
			if(!c) {
				puts("No such country");
				free(text_input);
				goto add_try_again;
			}
			n=malloc(sizeof(struct NEIGHBOR));
			n->name=strdup(c->name);
			n->country=c;
			n->next=country_edit->neighbor;
			country_edit->neighbor=n;
			n=malloc(sizeof(struct NEIGHBOR));
			n->name=strdup(country_edit->name);
			n->country=country_edit;
			n->next=c->neighbor;
			c->neighbor=n;
			printf("Added neighbor %s to country %s\n", c->name, country_edit->name);
			
			free(text_input);
			break;
		case '2':
			for(;;) {
				puts("Remove neighbor\nEnter a name");
				putchar('>');
				scanf("%m[^\n]", &text_input);
				getchar();
				if(!text_input) {
					puts(states[state].text);
					return;
				}
				for(nn=&country_edit->neighbor; *nn; nn=&((*nn)->next))
					if((*nn)->country) {
						if(!strcasecmp(text_input, (*nn)->country->name)) {
							printf("Removed neighbor %s from country %s\n", (*nn)->country->name, country_edit->name);
							n=*nn;
							*nn=n->next;
							for(nn=&n->country->neighbor; *nn; nn=&((*nn)->next))
								if((*nn)->country==country_edit) {
									p=*nn;
									*nn=(*nn)->next;
									free(p);
									break;
								}
							free(n);
							free(text_input);
							return;
						}
					} else
						if(!strcasecmp(text_input, (*nn)->name)) {
							n=*nn;
							*nn=n->next;
							free(n);
							printf("Removed orphaned neighbor %s from country %s\n", text_input, country_edit->name);
							free(text_input);
							return;
						}
					puts("No such neighbor");
				}
			break;
		case '3':
			puts(country_edit->name);
			for(n=country_edit->neighbor; n; n=n->next)
				if(n->country)
					printf("\tNeighbor: %s\n", n->country->name);
				else
					printf("\tOrphaned neighbor: %s\n", n->name);
			break;
		case '4':
			switch_state(prevstate);
			break;
		default:
			menu_common(input);
			break;
	}
}

static void db_load() {
	FILE *f;
	struct COUNTRY *c=NULL, *cn;
	struct NEIGHBOR *n;
	char *s, *ns;
	if(!(f=fopen("countries", "r"))) {
		puts("Warning: unable to open country database\n");
		return;
	}
	while(!feof(f)) {
		fscanf(f, "%m[^\n]", &s);
		getc(f);
		if(!s)
			continue;
		if(!*s) {
			free(s);
			//getc(f);
			continue;
		}
		if(*s!='\t') {
			c=malloc(sizeof(struct COUNTRY));
			c->name=s;
			c->neighbor=NULL;
			c->next=country;
			country=c;
			continue;
		}
		if(!c)
			continue;
		
		sscanf(s, " Capitol: %m[^\n]", &(c->capitol));
		sscanf(s, " Population: %lu", &(c->population));
		sscanf(s, " Area: %lu", &(c->area));
		if(sscanf(s, " Neighbor: %m[^\n]", &ns)) {
			n=malloc(sizeof(struct NEIGHBOR));
			n->country=NULL;
			n->name=ns;
			n->next=c->neighbor;
			c->neighbor=n;
		}
		
		free(s);
	}
	
	for(c=country; c; c=c->next) {
		for(n=c->neighbor; n; n=n->next) {
			for(cn=country; cn; cn=cn->next)
				if(!strcmp(cn->name, n->name)) {
					n->country=cn;
					break;
				}
			if(!cn)
				printf("Warning: Found orphaned neighbor %s to country %s\n", n->name, c->name);
		}
	}
	fclose(f);
}

static void db_save() {
	FILE *f;
	struct COUNTRY *c;
	struct NEIGHBOR *n;
	if(!(f=fopen("countries", "w"))) {
		puts("Warning: Unable to save database file\n");
		return;
	}
	
	for(c=country; c; c=c->next) {
		fprintf(f, "%s\n", c->name);
		fprintf(f, "\tCapitol: %s\n", c->capitol);
		fprintf(f, "\tPopulation: %lu\n", c->population);
		fprintf(f, "\tArea: %lu\n", c->area);
		for(n=c->neighbor; n; n=n->next) {
			fprintf(f, "\tNeighbor: %s\n", n->country?n->country->name:n->name);
		}
	}
}

int main(int argc, char **argv) {
	struct termios ttystate, ttysave;
	char c;
	srand(time(NULL));
	tcgetattr(STDIN_FILENO, &ttystate);
	ttysave=ttystate;
	ttystate.c_lflag&=~(ICANON);
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	
	db_load();
	
	switch_state(STATE_MAIN_MENU);
	for(;;) {
		printf(">");
		states[state].handler();
	}
	
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
	return 0;
}
