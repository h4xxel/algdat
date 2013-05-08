#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <ctype.h>

#define INFO_TABLE 64
#define PEOPLE_TABLE 1024

void add_person();
void remove_person();
void show_person();
void edit_person();
void list_database();

struct MENU {
	const char *text;
	void (*action)();
} menu[]={
	{"1. Add person", add_person},
	{"2. Remove person", remove_person},
	{"3. Show person", show_person},
	{"4. Edit person", edit_person},
	{"5. List database", list_database},
	{"6. Quit", NULL},
};

struct INFO {
	char const *key;
	char *value;
	unsigned int const hash;
	struct INFO *next;
};

struct PERSON {
	struct NAME {
		char *first;
		char *last;
	} const name;
	struct INFO *info[INFO_TABLE];
	unsigned int const hash;
	struct PERSON *next;
} *people[PEOPLE_TABLE];

unsigned int hash_string(char *string) {
	unsigned int hash=0;
	unsigned short i;
	for(i=1; *string; string++, hash+=i*tolower(*string), i*=31);
	return hash;
}

struct PERSON *db_find_person(const char *first_name, const char *last_name) {
	char *string=malloc(strlen(first_name)+strlen(last_name)+3);
	unsigned int hash;
	struct PERSON *person;
	sprintf(string, "%s, %s", last_name, first_name);
	hash=hash_string(string);
	free(string);
	for(person=people[hash%PEOPLE_TABLE]; person; person=person->next) {
		if(strcasecmp(first_name, person->name.first))
			continue;
		if(strcasecmp(last_name, person->name.last))
			continue;
		return person;
	}
	return NULL;
}

struct PERSON *db_add_person(char *first_name, char *last_name) {
	char *string=malloc(strlen(first_name)+strlen(last_name)+3);
	unsigned int hash;
	struct PERSON **person;
	sprintf(string, "%s, %s", last_name, first_name);
	hash=hash_string(string);
	free(string);
	for(person=&people[hash%PEOPLE_TABLE]; *person; person=&(*person)->next);
	*person=calloc(sizeof(struct PERSON), 1);
	*((char **) &((*person)->name.first))=first_name;
	*((char **) &((*person)->name.last))=last_name;
	*((int *) &((*person)->hash))=hash;
	return *person;
}

void db_remove_person(const char *first_name, const char *last_name) {
	char *string=malloc(strlen(first_name)+strlen(last_name)+3);
	unsigned int hash;
	int i;
	struct PERSON **person, *tmp;
	struct INFO *info, *next;
	sprintf(string, "%s, %s", last_name, first_name);
	hash=hash_string(string);
	free(string);
	for(person=&people[hash%PEOPLE_TABLE]; *person; person=&((*person)->next)) {
		if(strcasecmp(first_name, (*person)->name.first))
			continue;
		if(strcasecmp(last_name, (*person)->name.last))
			continue;
		tmp=*person;
		*person=tmp->next;
		free(tmp->name.first);
		free(tmp->name.last);
		for(i=0; i<INFO_TABLE; i++)
			for(info=tmp->info[i]; info; info=next) {
				next=info->next;
				free((void *) info->key);
				free(info->value);
				free(info);
			}
		free(tmp);
		return;
	}
}

void db_add_info(struct PERSON *person, char *key, char *value) {
	unsigned int hash=hash_string(key);
	struct INFO *info, *next;
	next=person->info[hash%INFO_TABLE];
	info=malloc(sizeof(struct INFO));
	*((char **) &(info->key))=key;
	*((int *) &(info->hash))=hash;
	info->value=value;
	info->next=next;
	person->info[hash%INFO_TABLE]=info;
}

int db_set_or_add_info(struct PERSON *person, char *key, char *value) {
	unsigned int hash=hash_string(key);
	struct INFO **info, *newinfo;
	for(info=&person->info[hash%INFO_TABLE]; *info; info=&(*info)->next) {
		if(!strcasecmp(key, (*info)->key)) {
			free((*info)->value);
			(*info)->value=value;
			return 0;
		}
	}
	newinfo=malloc(sizeof(struct INFO));
	*((char **) &(newinfo->key))=key;
	*((int *) &(newinfo->hash))=hash;
	newinfo->value=value;
	newinfo->next=*info;
	*info=newinfo;
	return 1;
}

void db_remove_info(struct PERSON *person, char *key) {
	unsigned int hash=hash_string(key);
	struct INFO **info, *tmp;
	for(info=&person->info[hash%INFO_TABLE]; *info; info=&(*info)->next) {
		if(strcasecmp(key, (*info)->key))
			continue;
		tmp=*info;
		*info=tmp->next;
		free((void *) tmp->key);
		free(tmp->value);
		free(tmp);
		return;
	}
}

void dump_info(const char *first_name, const char *last_name, FILE *stream) {
	struct PERSON *person;
	struct INFO *info;
	unsigned int i;
	if(!(person=db_find_person(first_name, last_name))) {
		puts("No person by that name found in database");
		return;
	}
	printf("%s, %s\n", person->name.last, person->name.first);
	for(i=0; i<INFO_TABLE; i++)
		for(info=person->info[i]; info; info=info->next)
			printf("%s: %s\n", info->key, info->value);
}

void dump_all(FILE *stream) {
	struct PERSON *person;
	struct INFO *info;
	unsigned int i, j;
	for(j=0; j<PEOPLE_TABLE; j++)
		for(person=people[j]; person; person=person->next) {
			fprintf(stream, "Last name: %s\nFirst name: %s\n", person->name.last, person->name.first);
			for(i=0; i<INFO_TABLE; i++)
				for(info=person->info[i]; info; info=info->next)
					fprintf(stream, "%s: %s\n", info->key, info->value);
			putc('\n', stream);
		}
}

void dump_all_sorted(FILE *stream) {
	struct PERSON *person, **tmp, *sorted=NULL, *next;
	struct INFO *info;
	unsigned int i, j;
	char *name1, *name2;
	for(j=0; j<PEOPLE_TABLE; j++)
		for(person=people[j]; person; person=person->next) {
			name1=malloc(strlen(person->name.first)+strlen(person->name.last)+3);
			sprintf(name1, "%s, %s", person->name.last, person->name.first);
			for(tmp=&sorted; *tmp; tmp=&((*tmp)->next)) {
				name2=malloc(strlen((*tmp)->name.first)+strlen((*tmp)->name.last)+3);
				sprintf(name2, "%s, %s", (*tmp)->name.last, (*tmp)->name.first);
				if(strcasecmp(name1, name2)<0) {
					free(name2);
					break;
				}
				free(name2);
			}
			free(name1);
			next=*tmp;
			*tmp=malloc(sizeof(struct PERSON));
			memcpy(*tmp, person, sizeof(struct PERSON));
			(*tmp)->next=next;
		}
	
	for(person=sorted; person; person=sorted) {
		fprintf(stream, "Last name: %s\nFirst name: %s\n", person->name.last, person->name.first);
		for(i=0; i<INFO_TABLE; i++)
			for(info=person->info[i]; info; info=info->next)
				fprintf(stream, "%s: %s\n", info->key, info->value);
		sorted=person->next;
		free(person);
		if(sorted)
			putc('\n', stream);
	}
}

void db_save() {
	FILE *f;
	if(!(f=fopen("people", "w"))) {
		puts("Warning: unable to save to database");
		return;
	}
	dump_all_sorted(f);
	fclose(f);
}

void db_load() {
	char *last_name;
	char *key, *value;
	FILE *f;
	struct PERSON *person;
	if(!(f=fopen("people", "r"))) {
		puts("Warning: unable to open database");
		return;
	}
	for(;;) {
		fscanf(f, "%m[^:\n]", &key);
		if(getc(f)=='\n') {
			free(key);
			continue;
		}
		if(feof(f)) {
			free(key);
			break;
		}
		getc(f);
		fscanf(f, "%m[^\n]", &value);
		getc(f);
		if(!strcasecmp(key, "Last name")) {
			last_name=value;
			free(key);
			continue;
		}
		if(!strcasecmp(key, "First name")) {
			person=db_add_person(value, last_name);
			free(key);
			continue;
		}
		db_add_info(person, key, value);
	}
	fclose(f);
}

void add_person() {
	char *first_name=NULL, *last_name=NULL, *string;
	printf("First name:\n>");
	scanf("%m[^\n]", &first_name);
	getchar();
	if(!first_name)
		return;
	printf("Last name:\n>");
	scanf("%m[^\n]", &last_name);
	getchar();
	if(last_name)
		db_add_person(first_name, last_name);
	else
		free(first_name);
}

void remove_person() {
	char *first_name=NULL, *last_name=NULL, *string;
	printf("First name:\n>");
	scanf("%m[^\n]", &first_name);
	getchar();
	if(!first_name)
		return;
	printf("Last name:\n>");
	scanf("%m[^\n]", &last_name);
	getchar();
	if(!last_name) {
		free(first_name);
		return;
	}
	if(last_name)
		db_remove_person(first_name, last_name);
	free(first_name);
	free(last_name);
}

void show_person() {
	char *first_name=NULL, *last_name=NULL, *string;
	printf("First name:\n>");
	scanf("%m[^\n]", &first_name);
	getchar();
	if(!first_name)
		return;
	printf("Last name:\n>");
	scanf("%m[^\n]", &last_name);
	getchar();
	if(last_name)
		dump_info(first_name, last_name, stdout);
	free(first_name);
	free(last_name);
}

void edit_person() {
	char *first_name=NULL, *last_name=NULL, *string;
	struct PERSON *person;
	printf("First name:\n>");
	scanf("%m[^\n]", &first_name);
	getchar();
	if(!first_name)
		return;
	printf("Last name:\n>");
	scanf("%m[^\n]", &last_name);
	getchar();
	if(last_name&&(person=db_find_person(first_name, last_name)))
		for(;;) {
			char *key;
			char *value;
			printf("Property:\n>");
			scanf("%m[^\n]", &key);
			getchar();
			if(!key)
				break;
			printf("Value:\n>");
			scanf("%m[^\n]", &value);
			getchar();
			if(!value) {
				db_remove_info(person, key);
				free(key);
				continue;
			}
			if(!db_set_or_add_info(person, key, value))
				free(key);
		}
	free(first_name);
	free(last_name);
}

void list_database() {
	dump_all_sorted(stdout);
}

void print_menu() {
	int i;
	for(i=0; i<sizeof(menu)/sizeof(struct MENU); i++)
		printf("\t%s\n", menu[i].text);
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
	puts("\"People\" 2013 Axel Isaksson");
	print_menu();
	for(;;) {
		putchar('>');
		c=getchar();
		putchar('\n');
		switch(c) {
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
				menu[c-'1'].action();
			case '\n':
				print_menu();
				break;
			case '6':
			case 'q':
			case 'Q':
				goto quit;
			case 's':
			case 'S':
				db_save();
				break;
			default:
				puts("?");
		}
	}
	quit:
	db_save();
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
	return 0;
}
