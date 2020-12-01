#include <stdlib.h>
#include <stdbool.h>

#include "mask.h"

#define MAX_NUMBER_LENGTH 4
#define MAX_MASK_NUMBER 8


int get_next_mask(FILE *masks_file, Mask *mask) {
	char temp_string[MAX_NUMBER_LENGTH + 1]; // null at the end of the buffer
	char temp_char;
	int i;

	for (i = 0; i < 4; ++i) {
		int offset = 0;
		while ((temp_char = fgetc(masks_file)) != EOF) {
			if (temp_char == '\n' || temp_char == ' ') {
				break;
			}
			temp_string[offset] = temp_char;
			offset++;
		}
		if (offset == 0) {
			return 1;
		}
		temp_string[offset] = 0;
		
		int temp_int = atoi(temp_string);

		switch(i) {
			case 0: mask->start_i = temp_int; break;
			case 1: mask->start_j = temp_int; break;
			case 2: mask->end_i = temp_int; break;
			case 3: mask->end_j = temp_int; break;
		}
	}

	if (i < 4) {
		return 1;
	}

	return 0;
}


Mask* get_all_masks(char *masks_file_path, int *masks_number) {
	Mask *masks = malloc(sizeof(Mask) * MAX_MASK_NUMBER);
	*masks_number = 0;
	FILE *masks_file = fopen(masks_file_path, "r");

	while (get_next_mask(masks_file, &masks[*masks_number]) == 0) {
		*masks_number = *masks_number + 1;
	}

	fclose(masks_file);
	return masks;
}


bool is_pixel_in_masks(int masks_number, Mask* masks, int i, int j) {
	for (int mask = 0; mask < masks_number; ++mask) {
		if (masks[mask].start_i <= i && i <= masks[mask].end_i && masks[mask].start_j <= j && j <= masks[mask].end_j) {
			return true;
		}
	}
	return false;
}
