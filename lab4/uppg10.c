#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct MORSE_TREE {
	char c;
	struct MORSE_TREE *dah;
	struct MORSE_TREE *dit;
} *morse_to_char_lookup;

char *char_to_morse_lookup[127]={
	[' ']="",
	['0']="-----",
	['1']=".----",
	['2']="..---",
	['3']="...--",
	['4']="....-",
	['5']=".....",
	['6']="-....",
	['7']="--...",
	['8']="---..",
	['9']="----.",
	['a']=".-",
	['b']="-...",
	['c']="-.-.",
	['d']="-..",
	['e']=".",
	['f']="..-.",
	['g']="--.",
	['h']="....",
	['i']="..",
	['j']=".---",
	['k']="-.-",
	['l']=".-..",
	['m']="--",
	['n']="-.",
	['o']="---",
	['p']=".--.",
	['q']="--.-",
	['r']=".-.",
	['s']="...",
	['t']="-",
	['u']="..-",
	['v']="...-",
	['w']=".--",
	['x']="-..-",
	['y']="-.--",
	['z']="--..",
};

struct MORSE_TREE *morse_tree_alloc(char c, struct MORSE_TREE *dah, struct MORSE_TREE *dit) {
	struct MORSE_TREE *ret=malloc(sizeof(struct MORSE_TREE));
	ret->c=c;
	ret->dah=dah;
	ret->dit=dit;
	return ret;
}

void generate_lookups() {
	morse_to_char_lookup=morse_tree_alloc(
		0,
		morse_tree_alloc('t', 
			morse_tree_alloc('m', 
				morse_tree_alloc('o', 
					morse_tree_alloc(0, 
						morse_tree_alloc('0', NULL, NULL), 
						morse_tree_alloc('9', NULL, NULL)
					), morse_tree_alloc(0, 
						NULL, 
						morse_tree_alloc('8', NULL, NULL)
					)
				), morse_tree_alloc('g',
					morse_tree_alloc('q', NULL, NULL),
					morse_tree_alloc('z',
						NULL,
						morse_tree_alloc('7', NULL, NULL)
					)
				)
			), morse_tree_alloc('n',
				morse_tree_alloc('k',
					morse_tree_alloc('y', NULL, NULL),
					morse_tree_alloc('c', NULL, NULL)
				), morse_tree_alloc('d',
					morse_tree_alloc('x', NULL, NULL),
					morse_tree_alloc('b' ,
						NULL,
						morse_tree_alloc('6', NULL, NULL)
					)
				)
			)
		), morse_tree_alloc('e',
			morse_tree_alloc('a',
				morse_tree_alloc('w',
					morse_tree_alloc('j',
						morse_tree_alloc('1', NULL, NULL),
						NULL
					), morse_tree_alloc('p', 
						NULL, 
						NULL
					)
				), morse_tree_alloc('r',
					NULL,
					morse_tree_alloc('l', NULL, NULL)
				)
			), morse_tree_alloc('i',
				morse_tree_alloc('u',
					morse_tree_alloc(0,
						morse_tree_alloc('2', NULL, NULL),
						NULL
					), morse_tree_alloc('f',
						NULL,
						NULL
					)
				), morse_tree_alloc('s',
					morse_tree_alloc('v',
						morse_tree_alloc('3', NULL, NULL),
						NULL
					), morse_tree_alloc('h',
						morse_tree_alloc('4', NULL, NULL),
						morse_tree_alloc('5', NULL, NULL)
					)
				)
			)
		)
	);
}

char morse_to_char(const char *morse) {
	struct MORSE_TREE *tree;
	
	for(tree=morse_to_char_lookup; tree; morse++) {
		switch(*morse) {
			case '.':
				tree=tree->dit;
				continue;
			case '-':
				tree=tree->dah;
			case ' ':
				continue;
			default:
				return 0;
		}
	}
	if(tree)
		return tree->c;
	return 0;
}

void print_cleartext(const char *morse) {
	struct MORSE_TREE *tree;
	int spaces;
	for(;;)
		for(tree=morse_to_char_lookup, spaces=0; tree; morse++) {
			switch(*morse) {
				case '.':
					tree=tree->dit;
					continue;
				case '-':
					tree=tree->dah;
					continue;
				case ' ':
					if(*(morse-1)==' ')
						putchar(' ');
					else
						putchar(tree->c);
					morse++;
					break;
				default:
					putchar(tree->c);
					return;
			}
			break;
		}
}

void print_morse(const char *string) {
	for(; *string; string++)
		printf("%s ", char_to_morse_lookup[*string]);
}

int main(int argc, char **argv) {
	if(argc!=3||*argv[1]!='-')
		return 1;
	generate_lookups();
	switch(argv[1][1]) {
		case 'c':
			print_cleartext(argv[2]);
			break;
		case 'm':
			print_morse(argv[2]);
			break;
		default:
			return 1;
	}
	putchar('\n');
	return 0;
}
