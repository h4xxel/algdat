#include <stdio.h>
#include <darnit/darnit.h>

/* I am lazy */
static const unsigned int color[]={
	0xFF0000FF,
	0xFF00FF00,
	0xFFFF0000,
	
	0xFF00FFFF,
	0xFFFFFF00,
	0xFFFF00FF,
	
	0xFF00007F,
	0xFF007F00,
	0xFF7F0000,
	
	0xFF007F7F,
	0xFF7F7F00,
	0xFF7F007F,
};

int floodfill(DARNIT_IMAGE_DATA *img, int x, int y, unsigned int from, unsigned int to) {
	if(x<0||x>=img->w||y<0||y>=img->h)
		return -1;
	if(img->data[y*img->w+x]!=from)
		return -1;
	img->data[y*img->w+x]=to;
	floodfill(img, x+1, y, from, to);
	floodfill(img, x-1, y, from, to);
	floodfill(img, x, y+1, from, to);
	floodfill(img, x, y-1, from, to);
	return 0;
}

int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA img;
	DARNIT_TILESHEET *t;
	int x, y, i=0;
	
	d_init_partial("uppg19");
	
	if(argc!=2) {
		fprintf(stderr, "Usage: uppg19 image.(png|jpg|gif|bmp)\n");
		return 1;
	}
	if(!(img=d_img_load_raw(argv[1])).data) {
		fprintf(stderr, "Fatal: invalid image file\n");
		return 1;
	}
	
	d_init_rest("uppg19", img.w, img.h, 0, NULL);
	
	for(y=0; y<img.h; y++)
		for(x=0; x<img.w; x++)
			if(floodfill(&img, x, y, 0xFF000000, 0xFF000000|color[i])>=0)
				i=(i+1)%(sizeof(color)/sizeof(unsigned int));
	
	t=d_render_tilesheet_new(1, 1, img.w, img.h, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_update(t, 0, 0, img.w, img.h, img.data);
	free(img.data);
	
	for(;;) {
		d_render_begin();
		d_render_tile_blit(t, 0, 0, 0);
		d_render_end();
		d_loop();
	}
	
	d_render_tilesheet_free(t);
	d_quit();
	return 0;
}