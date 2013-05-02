#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

int evaluate(char **token, int tokens, double *value, int levels) {
	double a=*value;
	double b=atof(token[1]);
	if(token[0][1])
		return -1;
	switch(*token[0]) {
		case '-':
			b*=-1;
		case '+':
			if(tokens>2)
				if(*token[2]=='*'||*token[2]=='/')
					levels+=evaluate(token+2, tokens-2, &b, levels);
			a+=b;
			break;
		case '*':
			a*=b;
			if(tokens>2)
				if(*token[2]=='*'||*token[2]=='/')
					levels+=evaluate(token+2, tokens-2, &a, levels);
			break;
		case '/':
			if(!b)
				return -1;
			a/=b;
			if(tokens>2)
				if(*token[2]=='*'||*token[2]=='/')
					levels+=evaluate(token+2, tokens-2, &a, levels);
			break;
		default:
			return -1;
	}
	*value=a;
	return levels+1;
}

double calculate(char *expression, double *result) {
	char *token[64];
	int i, j;
	int b;
	double value;
	int levels;
	for(i=0; ; i++, expression++) {
		token[i]=expression;
		do {
			if(!*expression)
				goto end_tokenize;
			expression++;
		} while(!isspace(*expression));
		*expression=0;
	}
	end_tokenize:
	if(i%2)
		return -1;
	value=atof(token[0]);
	for(j=1; j<i; j+=2*levels)
		if((levels=evaluate(token+j, i-j, &value, 0))<0)
			return -1;
	
	*result=value;
	return 0;
}

int main(int argc, char **argv) {
	char *expression;
	double result;
	printf("> ");
	while(!feof(stdin)) {
		expression=NULL;
		scanf("%m[^\n]", &expression);
		if(!expression) {
			getchar();
			continue;
		}
		printf("%s = ", expression);
		if(calculate(expression, &result)<0)
			printf("Invalid expression\n");
		else 
			printf("%g\n", result);
		free(expression);
		printf("> ");
	}
	return 0;
}
