#include <stdio.h>
#include <stdlib.h>
#include <darnit/darnit.h>

enum DIRECTION {
	DIRECTION_LEFT=0x1000,
	DIRECTION_RIGHT=0x0100,
	DIRECTION_UP=0x0010,
	DIRECTION_DOWN=0x0001,
};

enum TILE {
	TILE_WALL,
	TILE_PATH,
	TILE_SUCCESSFUL,
	TILE_FAILED,
};

struct COORD {
	int x;
	int y;
} start;

int generate_path(DARNIT_TILEMAP *map, int x, int y, int length, enum DIRECTION direction) {
	int i, dx, dy;
	if(!direction)
		return 0;
	dx=direction==DIRECTION_RIGHT?1:direction==DIRECTION_LEFT?-1:0;
	dy=direction==DIRECTION_DOWN?1:direction==DIRECTION_UP?-1:0;
	for(; length; length--) {
		if(x<0||x>=map->w||y<0||y>=map->h)
			return 1;
		start.x=x;
		start.y=y;
		map->data[y*map->w+x]=TILE_PATH;
		x+=dx;
		y+=dy;
	}
	i=rand();
	if(generate_path(map, x, y, rand()%5, i&DIRECTION_LEFT))
		return 1;
	if(generate_path(map, x, y, rand()%5, i&DIRECTION_RIGHT))
		return 1;
	if(generate_path(map, x, y, rand()%5, i&DIRECTION_DOWN))
		return 1;
	if(generate_path(map, x, y, rand()%5, i&DIRECTION_UP))
		return 1;
	return 0;
}

void generate(DARNIT_TILEMAP *map) {
	while(!generate_path(map, 800/64, 600/64, 2, DIRECTION_RIGHT));
	while(!generate_path(map, 800/64, 600/64, 2, DIRECTION_LEFT));
}

void find(DARNIT_TILEMAP *map, int x, int y) {
	if(x<0||y<0||x>=map->w||y>=map->h)
		return;
	if(map->data[y*map->w+x]!=TILE_PATH)
		return;
	map->data[y*map->w+x]=TILE_SUCCESSFUL;
	find(map, x-1, y);
	find(map, x+1, y);
	find(map, x, y-1);
	find(map, x, y+1);
	return;
}

static const unsigned int color[]={
	0xFF7F7F7F,
	0xFF202020,
	0xFF00FF00,
	0xFF0000FF,
};

int main() {
	int i;
	DARNIT_TILESHEET *t;
	DARNIT_TILEMAP *map;
	unsigned int ts_data[32*32*4];
	
	d_init_custom("uppg23", 32*40, 32*20, 0, "uppg23", NULL);
	srand(time(NULL));
	
	for(i=0; i<sizeof(ts_data)/sizeof(unsigned int); i++)
		ts_data[i]=color[(i%128)/32];
	t=d_render_tilesheet_new(4, 1, 32, 32, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_update(t, 0, 0, 32*4, 32, ts_data);
	
	map=d_tilemap_new(0xFFF, t, 0xFFF, 800/32, 600/32);
	generate(map);
	find(map, start.x, start.y);
	map->data[start.y*map->w+start.x]=TILE_FAILED;
	d_tilemap_recalc(map);
	
	for(;;) {
		d_render_begin();
		d_tilemap_draw(map);
		d_render_end();
		d_loop();
	}
	d_quit();
	return 0;
}