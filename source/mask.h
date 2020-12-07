#ifndef MASK_H
#define MASK_H


#include <stdio.h>
#include <stdbool.h>


typedef struct {
	int start_i, start_j, end_i, end_j;
} Mask;


int get_all_masks(Mask **masks, char *masks_file_path, int max_width, int max_height);


bool is_pixel_in_masks(int masks_number, Mask* masks, int i, int j);


int get_max_number_lenght(int max);


#endif
