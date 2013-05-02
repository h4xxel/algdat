#include <stdio.h>
#include <stdlib.h>

const char *suit[]={
	"spades",
	"hearts",
	"diamonds",
	"clubs",
};

const char *face[]={
	"Ace",
	"Two",
	"Three",
	"Four",
	"Five",
	"Six",
	"Seven",
	"Eight",
	"Nine",
	"Ten",
	"Knight",
	"Queen",
	"King",
};

const char **fortune;
unsigned int fortunes;

unsigned char deck[52];
unsigned char *top=deck;

void shuffle(unsigned int times) {
	unsigned char card, i1, i2;
	for(; times; times--) {
		i1=rand()%52;
		i2=rand()%52;
		card=deck[i1];
		deck[i1]=deck[i2];
		deck[i2]=card;
	}
}

unsigned char pop() {
	unsigned char card=*top;
	top++;
	return card;
}

void push(unsigned char card) {
	top--;
	*top=card;
}

void print(unsigned char card) {
	printf("\t%s of %s\n", face[card/4], suit[card%4]);
}

void load_fortunes() {
	FILE *f;
	unsigned int i=0;
	char *text;
	if(!(f=fopen("fortunes", "r")))
		exit(1);
	for(i=0; !feof(f); i++) {
		fscanf(f, "%*[^\n]");
		getc(f);
	}
	rewind(f);
	fortunes=i;
	fortune=malloc(sizeof(void *)*i);
	for(i=0; !feof(f); i++) {
		fscanf(f, "%m[^\n]", &fortune[i]);
		getc(f);
	}
	fclose(f);
}

int main(int argc, char **argv) {
	int i;
	unsigned char card[6];
	srand(time(NULL));
	load_fortunes();
	for(i=0; i<52; i++)
		deck[i]=i;
	while(getchar()!='q') {
		shuffle(1000);
		puts("You draw the cards:");
		for(i=0; i<6; i++)
			print(card[i]=pop());
		printf(
			"\nThe fortune teller looks at the cards for a while, chuckles and whispers in a mystical voice:\n\"%s\"\n",
			fortune[(card[0]+3*card[1]+card[2]+4*card[3]+card[4]+2*card[5])%fortunes]
		);
		for(i=0; i<6; i++)
			push(card[i]);
	}
}
