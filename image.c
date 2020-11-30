#include <stdio.h>
#include <stdlib.h>

#include "image.h"


Image* load_raw_image(char *filepath, int width, int height) {

	Image *image = malloc(sizeof(Image));

	unsigned char *data = malloc(width * height * sizeof(unsigned char));

	FILE* file = fopen(filepath, "rb");

	/*for (int i = 0; i < height; ++i) {
		data[i] = malloc(width * sizeof(unsigned char));
		fread(data[i], 1, width, file);
	}*/

	fread(data, sizeof(unsigned char), width * height, file);
	fclose(file);

	image->data = data;
	image->width = width;
	image->height = height;

	return image;
}


Image* copy_image(Image* base_image) {

	Image *image = malloc(sizeof(image));

	unsigned char* data = malloc(base_image->height * sizeof(unsigned char*));

	/*
	for (int i = 0; i < base_image->height; ++i) {
		data[i] = malloc(base_image->width * sizeof(unsigned char));

		for (int j = 0; j < base_image->width; ++j){
			data[i][j] = base_image->data[i][j];
		}
	}*/

	image->data = data;
	image->width = base_image->width;
	image->height = base_image->height;

	return image;
}


unsigned char get_pixel(Image* image, int i, int j) {
	// data[i][j] == *(data + i * c + j)
	// return image->data[i][j];
	return *(image->data + i * image->width + j);
}

void set_pixel(Image* image, int i, int j, unsigned char value) {
	*(image->data + i * image->width + j) = value;
}
