#ifndef IMAGE_H
#define IMAGE_H


typedef struct {
	unsigned char *data;
	int width;
	int height;
} Image;


void load_raw_image(char *filepath, int width, int height, Image *image);


unsigned char get_pixel(Image *image, int i, int j);


void set_pixel(Image *image, int i, int j, unsigned char value);


unsigned char get_blur_pixel(Image *image, int pixel_i, int pixel_j, int n);


#endif