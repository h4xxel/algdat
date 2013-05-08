#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define WORD_TABLE 65536
#define MAX_WORDS 100

struct WORD {
	char const *word;
	unsigned int count;
	unsigned int const hash;
	struct WORD *next;
} *wordtable[WORD_TABLE];

unsigned int hash_string(const char *string) {
	unsigned int hash=0;
	unsigned short i;
	for(i=1; *string; string++, hash+=i*(*string), i*=31);
	return hash;
}

void count_word(const char *s) {
	unsigned int hash=hash_string(s);
	struct WORD **word, *newword;
	for(word=&wordtable[hash%WORD_TABLE]; *word; word=&(*word)->next) {
		if(!strcmp(s, (*word)->word)) {
			(*word)->count++;
			return;
		}
	}
	newword=malloc(sizeof(struct WORD));
	*((char **) &(newword->word))=strdup(s);
	*((int *) &(newword->hash))=hash;
	newword->count=1;
	newword->next=*word;
	*word=newword;
}

int main(int argc, char **argv) {
	char w[256];
	char c, *s=w;
	int i;
	struct WORD *word, *sorted=NULL, *next, **tmp;
	
	while(!feof(stdin)) {
		c=getchar();
		if(ispunct(c)||isdigit(c)||isspace(c)) {
			if(s>w) {
				if(c=='\'')
					goto inword;
				*s=0;
				count_word(w);
				s=w;
			}
			continue;
		}
		inword:
		*s=tolower(c);
		s++;
	}
	for(i=0; i<WORD_TABLE; i++)
		for(word=wordtable[i]; word; word=word->next) {
			for(tmp=&sorted; *tmp; tmp=&((*tmp)->next)) {
				if(word->count>=(*tmp)->count)
					break;
			}
			next=*tmp;
			*tmp=malloc(sizeof(struct WORD));
			memcpy(*tmp, word, sizeof(struct WORD));
			(*tmp)->next=next;
		}
	for(i=0, word=sorted; word; word=sorted) {
		if(i++<MAX_WORDS)
			printf("%s: %u\n", word->word, word->count);
		sorted=word->next;
		free(word);
	}
	return 0;
}
