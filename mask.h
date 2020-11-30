#include <stdio.h>

typedef struct {
	int start_i, start_j, end_i, end_j;
} Mask;


Mask* get_next_mask(FILE *masks_file);

Mask** get_all_masks(char *masks_file_path, int *masks_number);