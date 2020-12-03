#ifndef MASK_H
#define MASK_H


#include <stdio.h>
#include <stdbool.h>


typedef struct {
	int start_i, start_j, end_i, end_j;
} Mask;


int get_next_mask(FILE *masks_file, Mask*);


Mask* get_all_masks(char *masks_file_path, int *masks_number);


bool is_pixel_in_masks(int masks_number, Mask* masks, int i, int j);


#endif
