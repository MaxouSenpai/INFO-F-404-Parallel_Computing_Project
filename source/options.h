#ifndef OPTIONS_H
#define OPTIONS_H


typedef struct {
	char *image_filename;
	char *output_image_filename;
	char *masks_filename;
	int n;
	int width;
	int height;
} Options;


int parse_arguments(int argc, char *argv[], Options *options);


#endif
