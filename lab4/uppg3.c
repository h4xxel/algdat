#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <darnit/darnit.h>

#define SIZE 20
#define PARENT(i) (((i)-1)/2)
#define LEFT(i) (2*(i)+1)
#define RIGHT(i) (2*(i)+2)

unsigned int colors[]={
	0xFF0000, 0x00FF00, 0x0000FF, 0xFFFF00, 0x00FFFF, 0xFF00FF
};
int tree[SIZE];
int size;
unsigned int color[SIZE];
DARNIT_FONT *font;
DARNIT_CIRCLE *circle[SIZE];
DARNIT_TEXT_SURFACE *text[SIZE];
DARNIT_TEXT_SURFACE *max;
DARNIT_TEXT_SURFACE *min;
DARNIT_LINE *traverse;
DARNIT_LINE *path_maxmin;
int path_maxmin_lines;
int traverse_lines;
int max_index, min_index;

void insert(int value) {
	int i=0, x=400, y=30, add=200, c=0;
	char s[10];
	while(1)
		if(!tree[i]) {
			tree[i]=value;
			sprintf(s, "%i", value);
			d_render_circle_move(circle[size], x, y, 20);
			d_text_surface_string_append(text[i]=d_text_surface_new(font, 10, 64, x-10, y-6), s);
			size++;
			color[i]=colors[c];
			return;
		} else if(value<tree[i]) {
			y+=50;
			x-=add;
			add/=2;
			c++;
			i=LEFT(i);
			continue;
		} else if(value>tree[i]) {
			y+=50;
			x+=add;
			add/=2;
			c++;
			i=RIGHT(i);
			continue;
		} else
			return;
}

void loop() {
	int i, j;
	for(j=30; j; j--) {
		d_render_begin();
		d_render_tint(0x5F, 0x5F, 0x5F, 0xFF);
		for(i=0; i<path_maxmin_lines; i++)
			d_render_line_draw(path_maxmin, path_maxmin_lines);
		d_render_tint(0xFF, 0xFF, 0xFF, 0xFF);
		for(i=0; i<traverse_lines; i++)
			d_render_line_draw(traverse, traverse_lines);
		if(max)
			d_text_surface_draw(max);
		if(min)
			d_text_surface_draw(min);
		for(i=0; i<SIZE; i++) {
			d_render_tint(color[i], color[i]>>8, color[i]>>16, 0xFF);
			d_render_circle_draw(circle[i]);
			if(text[i])
				d_text_surface_draw(text[i]);
		}
		d_render_end();
		d_loop();
	}
}

void insert_many(int number, ...) {
	int i;
	va_list ap;
	va_start(ap, number);
	while(number--) {
		insert(va_arg(ap, int));
		loop();
	}
	va_end(ap);
}

void mark_path(int value) {
	int i=0, x=400, y=30, add=200;
	traverse_lines=0;
	while(1) {
		loop();
		if(!tree[i]) {
			traverse_lines=0;
			return;
		} else if(value<tree[i]) {
			d_render_line_move(traverse, traverse_lines++, x, y, x-add, y+50);
			x-=add;
			y+=50;
			add/=2;
			i=LEFT(i);
			continue;
		} else if(value>tree[i]) {
			d_render_line_move(traverse, traverse_lines++, x, y, x+add, y+50);
			x+=add;
			y+=50;
			add/=2;
			i=RIGHT(i);
			continue;
		} else
			return;
	}
}

void mark_max() {
	int i=0, x=400, y=30, add=200, c=0;
	traverse_lines=0;
	while(tree[i]) {
		max_index=i;
		loop();
		d_render_line_move(traverse, traverse_lines++, x, y, x+add, y+50);
		x+=add;
		y+=50;
		add/=2;
		i=RIGHT(i);
	}
	if(!i)
		return;
	traverse_lines=0;
	d_text_surface_free(max);
	d_text_surface_string_append(max=d_text_surface_new(font, 10, 64, x-add*2-12, y-30), "max");
}

void mark_min() {
	int i=0, x=400, y=30, add=200, c=0;
	traverse_lines=0;
	while(tree[i]) {
		min_index=i;
		loop();
		d_render_line_move(traverse, traverse_lines++, x, y, x-add, y+50);
		x-=add;
		y+=50;
		add/=2;
		i=LEFT(i);
	}
	if(!i)
		return;
	traverse_lines=0;
	d_text_surface_free(min);
	d_text_surface_string_append(min=d_text_surface_new(font, 10, 64, x+add*2-12, y-30), "min");
}

void mark_inorder(int root, int x, int y, int add) {
	int i;
	i=LEFT(root);
	if(i<size) {
		mark_inorder(i, x-add, y+50, add/2);
		loop();
		d_render_line_move(path_maxmin, path_maxmin_lines++, x, y, x-add, y+50);
	}
        i=RIGHT(root);
	if(i<size) {
		mark_inorder(i, x+add, y+50, add/2);
		loop();
		d_render_line_move(path_maxmin, path_maxmin_lines++, x, y, x+add, y+50);
	}
}

void mark_path_maxmin() {
	path_maxmin_lines=0;
	mark_inorder(0, 400, 30, 200);
}

int main(int argc, char **argv) {
	int i;
	d_init("Binary Search Tree", "uppg3", NULL);
	d_render_blend_enable();
	font=d_font_load("font.ttf", 12, 512, 512);
	traverse=d_render_line_new(SIZE, 3);
	path_maxmin=d_render_line_new(SIZE, 3);
	for(i=0; i<SIZE; i++)
		d_render_circle_move(circle[i]=d_render_circle_new(24, 1), 0, 0, 0);
	d_render_clearcolor_set(0x0, 0x0, 0x0);
	insert_many(19, 400, 200, 600, 100, 300, 500, 700, 50, 150, 250, 350, 450, 550, 650, 750, 25, 75, 125, 175);
	mark_min();
	mark_max();
	mark_path_maxmin();
	mark_path(150);
	for(;;)
		loop();
	d_quit();
	return 0;
}
