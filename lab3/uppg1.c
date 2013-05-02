#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

const char *menu=
	"Main menu\n\n"
	"	1. Add word\n"
	"	2. Remove word\n"
	"	3. Find synonyms\n"
	"	4. Link words\n"
	"	5. Quit";

struct SYNONYM {
	char *word;
	struct  SYNONYM *next;
};

struct DB {
	struct SYNONYM *synonym;
	struct DB *next;
} *db;

void find_synonyms(const char *word) {
	struct DB *d;
	struct SYNONYM *s;
	
	for(d=db; d; d=d->next)
		for(s=d->synonym; s; s=s->next)
			if(!strcasecmp(word, s->word)) {
				for(s=d->synonym; s; s=s->next)
					printf("\t%s\n", s->word);
				return;
			}
	puts("No synonyms found");
}

void remove_word(const char *word) {
	struct DB **dd, *d;
	struct SYNONYM **ss, *s;
	
	for(dd=&db; *dd; dd=&((*dd)->next))
		for(ss=&((*dd)->synonym); *ss; ss=&((*ss)->next))
			if(!strcasecmp(word, (*ss)->word)) {
				s=*ss;
				*ss=s->next;
				if(!((*dd)->synonym)) {
					d=*dd;
					*dd=d->next;
					free(d);
					puts("Removing empty chain");
				}
				free(s);
				return;
			}
	puts("Word not found");
}

void add_word(const char *word) {
	struct DB *d;
	struct SYNONYM *s;
	
	for(d=db; d; d=d->next)
		for(s=d->synonym; s; s=s->next)
			if(!strcasecmp(word, s->word)) {
				puts("Word already exists in database");
				return;
			}
	d=malloc(sizeof(struct DB));
	s=calloc(sizeof(struct SYNONYM), 1);
	s->word=strdup(word);
	d->synonym=s;
	d->next=db;
	db=d;
}

void link_words(const char *word1, const char *word2) {
	struct DB *d, **dd, **d1=NULL, *d2=NULL;
	struct SYNONYM *s, **ss;
	
	for(dd=&db; *dd; dd=&(*dd)->next)
		for(s=(*dd)->synonym; s; s=s->next) {
			if(!strcasecmp(word1, s->word))
				d1=dd;
			if(!strcasecmp(word2, s->word))
				d2=*dd;
		}
	if(!d1) {
		printf("Word %s does not exist in database\n", word1);
		return;
	}
	d=*d1;
	if(!d2) {
		printf("Word %s does not exist in database\n", word2);
		return;
	}
	if(d==d2) {
		printf("Words %s and %s are already synonyms\n", word1, word2);
		return;
	}
	
	for(ss=&d2->synonym; *ss; ss=&((*ss)->next));
	*ss=d->synonym;
	*d1=d->next;
	free(d);
}

void db_load() {
	FILE *f;
	char *word;
	struct DB *d;
	struct SYNONYM *s;
	if(!(f=fopen("synonyms", "r"))) {
		puts("Warning: unable to load database");
		return;
	}
	
	d=calloc(sizeof(struct DB), 1);
	while(!feof(f)) {
		fscanf(f, "%m[^\n]", &word);
		getc(f);
		if(!word) {
			d->next=db;
			db=d;
			d=calloc(sizeof(struct DB), 1);
			continue;
		}
		s=malloc(sizeof(struct SYNONYM));
		s->word=word;
		s->next=d->synonym;
		d->synonym=s;
	}
	free(d);
	fclose(f);
}

void db_save() {
	FILE *f;
	if(!(f=fopen("synonyms", "w"))) {
		puts("Warning: unable to save database");
		return;
	}
	
	struct DB *d;
	struct SYNONYM *s;
	
	for(d=db; d; d=d->next) {
		for(s=d->synonym; s; s=s->next)
			fprintf(f, "%s\n", s->word);
		if(d->next)
			fputc('\n', f);
	}
	
	fclose(f);
}

int main(int argc, char **argv) {
	struct DB *d;
	struct SYNONYM *s;
	struct termios ttystate, ttysave;
	char c, *word1, *word2;
	struct {
		const char *text;
		void (*action)(const char *word);
	} static const alt[]={
		{"Add word:", add_word},
		{"Remove word:", remove_word},
		{"Find synonyms\nEnter a word:", find_synonyms},
	};
	tcgetattr(STDIN_FILENO, &ttystate);
	ttysave=ttystate;
	ttystate.c_lflag&=~(ICANON);
	ttystate.c_cc[VMIN] = 1;
	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
	
	db_load();
	
	puts(menu);
	for(;;) {
		putchar('>');
		c=getchar();
		putchar('\n');
		switch(c) {
			case '1':
			case '2':
			case '3':
				do {
					puts(alt[c-'1'].text);
					putchar('>');
					scanf("%m[^\n]", &word1);
					getchar();
					if(word1)
						alt[c-'1'].action(word1);
					free(word1);
					putchar('\n');
				} while(word1);
			case '\n':
			case '?':
			case 'h':
			case 'H':
				puts(menu);
				break;
			case '4':
				word1=word2=NULL;
				for(;;) {
					free(word1);
					free(word2);
					word1=word2=NULL;
					puts("Link words\nEnter first word:");
					putchar('>');
					scanf("%m[^\n]", &word1);
					getchar();
					if(!word1)
						break;
					puts("Enter second word:");
					putchar('>');
					scanf("%m[^\n]", &word2);
					getchar();
					if(!word2)
						break;
					link_words(word1, word2);
				}
				free(word1);
				free(word2);
				puts(menu);
				break;
			case 's':
			case 'S':
				db_save();
				break;
			case '5':
			case 'q':
			case 'Q':
				goto end;
			default:
				puts("?");
		}
	}
	
	end:
	db_save();
	tcsetattr(STDIN_FILENO, TCSANOW, &ttysave);
	return 0;
}