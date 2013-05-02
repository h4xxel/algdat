#include <stdio.h>
#include <darnit/darnit.h>

int partition(unsigned int *array, int left, int right, int pivot) {
	int i, j;
	unsigned int a=array[pivot], b;
	array[pivot]=array[right];
	array[right]=a;
	for(i=j=left; i<right-1; i++)
		if(array[i]>a) {
			b=array[j];
			array[j]=array[i];
			array[i]=b;
			j++;
		}
	b=array[right];
	array[right]=array[j];
	array[j]=b;
	return j;
}

void quicksort(unsigned int *array, int left, int right) {
	if(left>=right)
		return;
	int i=partition(array, left, right, (left+right)/2);
	quicksort(array, left, i-1);
	quicksort(array, i+1, right);
}

void sort(unsigned int *array, int size) {
	quicksort(array, 0, size-1);
}

int main(int argc, char **argv) {
	DARNIT_IMAGE_DATA img;
	DARNIT_TILESHEET *t1, *t2, *t;
	int x, y, i=0;
	
	d_init_partial("uppg16");
	
	if(argc!=2) {
		fprintf(stderr, "Usage: uppg16 image.(png|jpg|gif|bmp)\n");
		return 1;
	}
	if(!(img=d_img_load_raw(argv[1])).data) {
		fprintf(stderr, "Fatal: invalid image file\n");
		return 1;
	}
	
	d_init_rest("uppg16", img.w, img.h, 0, NULL);
	
	t1=d_render_tilesheet_new(1, 1, img.w, img.h, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_update(t1, 0, 0, img.w, img.h, img.data);
	sort(img.data, img.w*img.h);
	t2=d_render_tilesheet_new(1, 1, img.w, img.h, DARNIT_PFORMAT_RGBA8);
	d_render_tilesheet_update(t2, 0, 0, img.w, img.h, img.data);
	free(img.data);
	
	for(t=t1;;) {
		if(d_keys_get().start) {
			t=t==t1?t2:t1;
			d_keys_set(d_keys_get());
		}
		d_render_begin();
		d_render_tile_blit(t, 0, 0, 0);
		d_render_end();
		d_loop();
	}
	
	d_render_tilesheet_free(t);
	d_quit();
	return 0;
}