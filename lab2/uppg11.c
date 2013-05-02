#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STRINGSIZE 64
#define PAPERS 50

#define POSTFIX_POPARG(operator) \
	stack--; \
	a=*stack; \
	stack--; \
	b=*stack; \
	*stack=b operator a; \
	stack++;

char *first_stack[PAPERS];
char *second_stack[PAPERS];

char **first;
char **second;

void push(char ***stack, char *paper) {
	**stack=paper;
	(*stack)++;
}

char *pop(char ***stack) {
	(*stack)--;
	return **stack;
}

int parse_postfix(char *string) {
	char *s;
	int *stack, *save;
	int a, b;
	if(!string)
		return 0;
	save=stack=malloc(512);
	while((s=strchr(string, ' '))||(s=strchr(string, '\0'))) {
		switch(*string) {
			case '+':
				if(!*(string+1)||*(string+1)!=' ')
					goto number;
				POSTFIX_POPARG(+);
				break;
			case '-':
				if(!*(string+1)||*(string+1)!=' ')
					goto number;
				POSTFIX_POPARG(-);
				break;
			case '*':
				POSTFIX_POPARG(*);
				break;
			case '/':
					stack--;
					a=*stack;
					if(!a) {
						fprintf(stderr, "Men fy på dig, Sanna, man får inte dela med noll!\n");
						free(save);
						return 0;
					}
					stack--;
					b=*stack;
					*stack=b / a;
					stack++;
				break;
			default:
			number:
				*s=0;
				*stack=atoi(string);
				stack++;
			case ' ':
				break;
		}
		if(!*(string=s+1))
			break;
	}
	stack--;
	a=*stack;
	free(save);
	return a;
}

void generate_postfix(char **string) {
	char *s=*string;
	unsigned int length=3+(rand()%2)*2;
	int numbers=0;
	unsigned int a, b;
	char op[]={
		'+', '-', '*', '/',
	};
	
	while(length) {
		a=rand()%6;
		if((numbers>=2&&a<4)||numbers>=length) {
			*s=op[a%4];
			s++;
			*s=' ';
			s++;
			numbers-=2;
		} else {
			b=rand()%3+1;
			while(b--) {
				a=rand()%10;
				*s='0'+a;
				s++;
				if(!a)
					break;
			}
			*s=' ';
			s++;
			numbers++;
		}
		length--;
	}
	*s=0;
}

int main(int argc, char **argv) {
	int i;
	srand(time(NULL));
	for(first=first_stack; first<first_stack+PAPERS; first++)
		*first=malloc(STRINGSIZE);
	second=second_stack;
	
	for(i=0; i<PAPERS; i++) {
		char *paper=pop(&first);
		generate_postfix(&paper);
		push(&second, paper);
	}
	
	for(i=0; i<PAPERS; i++) {
		char *paper=pop(&second);
		int a;
		printf("(%s) = ", paper);
		a=parse_postfix(paper);
		printf("%i\n", a);
		sprintf(paper, "%i\n", a);
		push(&first, paper);
	}
	
	return 0;
}
