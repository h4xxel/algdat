#include <stdio.h>
#include <string.h>
#include <darnit/darnit.h>

struct AREA {
	int size;
	unsigned int *image;
	struct AREA *next;
};

int floodextract(DARNIT_IMAGE_DATA *src, unsigned int *dest, int x, int y, unsigned int from, unsigned int to, int size) {
	if(x<0||x>=src->w||y<0||y>=src->h)
		return size;
	if(src->data[y*src->w+x]!=from)
		return size;
	dest[y*src->w+x]=from;
	src->data[y*src->w+x]=to;
	size=floodextract(src, dest, x+1, y, from, to, size);
	size=floodextract(src, dest, x-1, y, from, to, size);
	size=floodextract(src, dest, x, y+1, from, to, size);
	size=floodextract(src, dest, x, y-1, from, to, size);
	return size+1;
}

int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA img;
	DARNIT_TILESHEET *t;
	int x, y, size;
	unsigned int color=1;
	struct AREA *area=NULL, *a;
	
	d_init_partial("uppg21");
	if(argc!=2) {
		fprintf(stderr, "Usage: uppg21 image.(png|jpg|gif|bmp)\n");
		return 1;
	}
	if(!(img=d_img_load_raw(argv[1])).data) {
		fprintf(stderr, "Fatal: invalid image file\n");
		return 1;
	}
	
	d_init_rest("uppg21", img.w, img.h, 0, NULL);
	
	a=malloc(sizeof(struct AREA));
	a->size=0;
	a->image=malloc(sizeof(unsigned int)*img.w*img.h);
	memset(a->image, 0xFFFFFFFF, sizeof(unsigned int)*img.w*img.h);
	a->next=NULL;
	for(y=0; y<img.h; y++)
		for(x=0; x<img.w; x++) {
			if((size=floodextract(&img, a->image, x, y, 0xFF000000, color, 0))>0) {
				struct AREA **aa;
				a->size=size;
				for(aa=&area; *aa; aa=&(*aa)->next) {
					if(size>=(*aa)->size)
						break;
				}
				a->next=*aa;
				*aa=a;
				a=malloc(sizeof(struct AREA));
				a->size=0;
				a->image=malloc(sizeof(unsigned int)*img.w*img.h);
				memset(a->image, 0xFFFFFFFF, sizeof(unsigned int)*img.w*img.h);
				a->next=NULL;
				color++;
			}
		}
	free(a->image);
	free(a);
	a=area;
	t=d_render_tilesheet_new(1, 1, img.w, img.h, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_update(t, 0, 0, img.w, img.h, a->image);
	printf("Size: %i\n", a->size);
	free(img.data);
	
	for(;;) {
		if(d_keys_get().start) {
			if(!(a=a->next))
				a=area;
			d_render_tilesheet_update(t, 0, 0, img.w, img.h, a->image);
			printf("Size: %i\n", a->size);
			d_keys_set(d_keys_get());
		}
		d_render_begin();
		d_render_tile_blit(t, 0, 0, 0);
		d_render_end();
		d_loop();
	}
	
	while(area) {
		a=area;
		area=area->next;
		free(a->image);
		free(a);
	}
	d_render_tilesheet_free(t);
	d_quit();
	return 0;
}