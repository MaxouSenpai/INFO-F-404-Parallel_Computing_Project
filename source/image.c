#include <stdio.h>
#include <stdlib.h>

#include "image.h"


/*
 * This function loads a raw image.
 *
 * filepath - the path of the image
 * width - the width of the image
 * height - the height of the image
 * image - the image struct to update
 */
void load_raw_image(Image *image, char *filepath, int width, int height) {

	unsigned char *data = malloc(width * height * sizeof(unsigned char));

	FILE* file = fopen(filepath, "rb");

	fread(data, sizeof(unsigned char), width * height, file);
	fclose(file);

	image->data = data;
	image->width = width;
	image->height = height;
}


/*
 * This function returns the value of the specified pixel of a specified image.
 *
 * image - the image
 * i - the row of the pixel
 * j - the column of the pixel
 * returns the value of the pixel
 */
unsigned char get_pixel(Image *image, int i, int j) {
	return *(image->data + i * image->width + j);
}


/*
 * This function sets the value of the specified pixel of a specified image.
 *
 * image - the image
 * i - the row of the pixel
 * j - the column of the pixel
 * value - the value of the pixel
 */
void set_pixel(Image *image, int i, int j, unsigned char value) {
	*(image->data + i * image->width + j) = value;
}


/*
 * This function returns the blur value of the specified pixel of a specified image.
 *
 * image - the image
 * i - the row of the pixel
 * j - the column of the pixel
 * n - the n (neighbourhood) value for the blurring algorithm
 * returns the blur value of the pixel
 */
unsigned char get_blur_pixel(Image *image, int pixel_i, int pixel_j, int n) {

	int terms_number = 0;
	int sum = 0;

	for (int i = pixel_i - n + 1; i < pixel_i + n; ++i) {
		if (0 <= i && i < image->height) {
			for (int j = pixel_j - n + 1; j < pixel_j + n; ++j) {
				if (0 <= j && j < image->width) {
					terms_number++;
					sum = sum + get_pixel(image, i, j);
				}
			}
		}
	}
	return sum / terms_number;
}
