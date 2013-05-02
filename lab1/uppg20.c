#include <stdio.h>
#include <darnit/darnit.h>

int floodfill(DARNIT_IMAGE_DATA *img, int x, int y, unsigned int from, unsigned int to, int size) {
	if(x<0||x>=img->w||y<0||y>=img->h)
		return size;
	if(img->data[y*img->w+x]!=from)
		return size;
	img->data[y*img->w+x]=to;
	size=floodfill(img, x+1, y, from, to, size);
	size=floodfill(img, x-1, y, from, to, size);
	size=floodfill(img, x, y+1, from, to, size);
	size=floodfill(img, x, y-1, from, to, size);
	return size+1;
}

int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA img;
	DARNIT_TILESHEET *t;
	int x, y, size;
	unsigned int color=1;
	struct {
		int size;
		unsigned int color;
	} largest;
	
	d_init_partial("uppg20");
	
	if(argc!=2) {
		fprintf(stderr, "Usage: uppg20 image.(png|jpg|gif|bmp)\n");
		return 1;
	}
	if(!(img=d_img_load_raw(argv[1])).data) {
		fprintf(stderr, "Fatal: invalid image file\n");
		return 1;
	}
	
	d_init_rest("uppg20", img.w, img.h, 0, NULL);
	
	for(y=0; y<img.h; y++)
		for(x=0; x<img.w; x++)
			if((size=floodfill(&img, x, y, 0xFF000000, color, 0))>0) {
				//printf("area of size %i\n", size);
				if(size>largest.size) {
					largest.size=size;
					largest.color=color;
				}
				color++;
			}
	
	for(y=0; y<img.h; y++)
		for(x=0; x<img.w; x++)
			if(img.data[y*img.w+x]==largest.color)
				img.data[y*img.w+x]=0xFF00FFFF;
			else
				img.data[y*img.w+x]=0xFF7F007F;
	
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