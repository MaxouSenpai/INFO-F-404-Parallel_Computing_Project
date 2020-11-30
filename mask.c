#include <stdlib.h>
#include <stdbool.h>

#include "mask.h"

#define MAX_NUMBER_LENGTH 4
#define MAX_MASK_NUMBER 8

Mask* get_next_mask(FILE *masks_file) {
	char temp_string[MAX_NUMBER_LENGTH + 1]; // null at the end of the buffer
	char temp_char;

	Mask *mask = malloc(sizeof(mask));
	bool ok = true;
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
			return 0;
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
		return 0;
	}

	return mask;
}

Mask** get_all_masks(char *masks_file_path, int *masks_number) {
	Mask **masks = malloc(sizeof(Mask*) * MAX_MASK_NUMBER);
	*masks_number = 0;
	Mask *current_mask;
	FILE *masks_file = fopen(masks_file_path, "r");

	while ((current_mask = get_next_mask(masks_file)) != 0) {
		masks[*masks_number] = current_mask;
		*masks_number = *masks_number + 1;
	}
	fclose(masks_file);
	return masks;
}