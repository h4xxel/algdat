#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct SET {
	char element;
	struct SET *next;
};

struct LIST {
	struct SET *set;
	struct LIST *next;
};

struct LIST **subsets(char *set) {
	unsigned int stringlen=strlen(set), i;
	unsigned long a, b;
	unsigned long length=1UL<<stringlen;
	char *s;
	struct SET *tmpset1, *tmpset2;
	struct LIST **list=calloc(sizeof(void *), length+2), *tmplist;
	
	for(a=0; a<length; a++) {
		for(tmpset1=NULL, s=set, b=a, i=0; b; b>>=1, s++)
			if(b&1) {
				tmpset2=malloc(sizeof(struct SET));
				tmpset2->element=*s;
				tmpset2->next=tmpset1;
				tmpset1=tmpset2;
				i++;
			}
		tmplist=malloc(sizeof(struct LIST));
		tmplist->next=list[i];
		tmplist->set=tmpset1;
		list[i]=tmplist;
	}
	return list;
}

int main(int argc, char **argv) {
	struct LIST **list, *l;
	struct SET *set;
	if(argc!=2)
		return 1;
	printf("[\n\t");
	for(list=subsets(argv[1]); *list; list++) {
		puts("[");
		for(l=*list; l; l=l->next) {
			printf("\t\t{ ");
			for(set=l->set; set; set=set->next)
				printf(set->next?"%c, ":"%c ", set->element);
			puts("},");
		}
		printf("\t], ");
	}
	puts("\n]");
}