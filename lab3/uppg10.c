#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DIGIT {
	unsigned char digit;
	struct DIGIT *next;
	struct DIGIT *prev;
};

enum SIGNED {
	UNSIGNED,
	SIGNED,
};

struct NUMBER {
	struct DIGIT *first;
	struct DIGIT *last;
	enum SIGNED sign;
};

void append_digit(struct NUMBER *number, unsigned char digit) {
	struct DIGIT *d;
	if(!number)
		return;
	if(digit=='-'&&!number->first) {
		number->sign=SIGNED;
		return;
	}
	if(digit>9)
		return;
	d=malloc(sizeof(struct DIGIT));
	d->digit=digit;
	d->next=NULL;
	d->prev=number->last;
	if(number->last)
		number->last->next=d;
	else
		number->first=d;
	number->last=d;
}

void prepend_digit(struct NUMBER *number, unsigned char digit) {
	struct DIGIT *d;
	if(!number)
		return;
	if(digit=='-'&&!number->first) {
		number->sign=SIGNED;
		return;
	}
	if(digit>9)
		return;
	d=malloc(sizeof(struct DIGIT));
	d->digit=digit;
	d->next=number->first;
	d->prev=NULL;
	if(number->first)
		number->first->prev=d;
	else
		number->last=d;
	number->first=d;
}

void print_number(struct NUMBER *number) {
	struct DIGIT *d;
	if(!number)
		return;
	if(!number->first) {
		putchar('0');
		return;
	}
	if(number->sign)
		putchar('-');
	for(d=number->first; d; d=d->next)
		putchar(d->digit+'0');
}

struct NUMBER *add_number(struct NUMBER *n1, struct NUMBER *n2) {
	char carry=0, a;
	struct DIGIT *d1, *d2;
	struct NUMBER *res;
	res=calloc(sizeof(struct NUMBER), 1);
	if(n1->sign^n2->sign) {
		
	}
	d1=n1->last;
	d2=n2->last;
	while(d1||d2) {
		a=carry;
		if(d1) {
			a+=d1->digit;
			d1=d1->prev;
		}
		if(d2) {
			a+=d2->digit;
			d2=d2->prev;
		}
		carry=a/10;
		prepend_digit(res, a%10);
	}
	if(carry)
		prepend_digit(res, carry);
	return res;
}

struct NUMBER *subtract_number(struct NUMBER *n1, struct NUMBER *n2) {
	n1->sign=!n1->sign;
	return add_number(n1, n2);
}

struct NUMBER *multiply_number(struct NUMBER *n1, struct NUMBER *n2) {
	char carry, a;
	int i, j;
	struct DIGIT *d1, *d2, *dtmp, *next;
	struct NUMBER *res, *add, *tmp;
	res=calloc(sizeof(struct NUMBER), 1);
	add=calloc(sizeof(struct NUMBER), 1);
	
	for(i=0, d2=n2->last; d2; i++, d2=d2->prev) {
		carry=0;
		for(j=i; j; j--)
			prepend_digit(add, 0);
		for(d1=n1->last; d1; d1=d1->prev){
			a=d1->digit*d2->digit;
			a+=carry;
			carry=a/10;
			prepend_digit(add, a%10);
		}
		if(carry)
			prepend_digit(add, carry);
		tmp=add_number(res, add);
		for(dtmp=add->first; dtmp; dtmp=next) {
			next=dtmp->next;
			free(dtmp);
		}
		for(dtmp=res->first; dtmp; dtmp=next) {
			next=dtmp->next;
			free(dtmp);
		}
		free(res);
		res=tmp;
		add->first=add->last=NULL;
		add->sign=UNSIGNED;
	}
	res->sign=n1->sign^n2->sign;
	return res;
}

struct NUMBER *divide_number(struct NUMBER *n1, struct NUMBER *n2) {
	return n1;
}

int main(int argc, char **argv) {
	static struct NUMBER *n1, *n2;
	char c;
	struct NUMBER *(*operation)(struct NUMBER *n1, struct NUMBER *n2);
	
	if(!(argc==2&&argv[1][1]==0))
		goto error;
	
	switch(*argv[1]) {
		case '+':
			operation=add_number;
			break;
		case '-':
			operation=subtract_number;
			break;
		case '*':
			operation=multiply_number;
			break;
		case '/':
			operation=divide_number;
			break;
		default:
		error:
			puts("Must specify operation from set [+-*/]");
			return 1;
	}
		
	n1=calloc(sizeof(struct NUMBER), 1);
	n2=calloc(sizeof(struct NUMBER), 1);
	puts("Enter first number");
	putchar('>');
	while((c=getchar())!='\n')
		append_digit(n1, c=='-'?c:c-'0');
	puts("Enter second number");
	putchar('>');
	while((c=getchar())!='\n')
		append_digit(n2, c=='-'?c:c-'0');
	
	print_number(n1);
	putchar(*argv[1]);
	print_number(n2);
	putchar('=');
	print_number(operation(n1, n2));
	putchar('\n');
	return 0;
}