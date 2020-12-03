#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "mask.h"


/*
 * This function gets the next mask of the specified file and
 * updates the specified mask struct.
 *
 * masks_file - the file containing the masks
 * mask - the mask struct to update
 * max_width - the maximum value of j
 * max_height - the maximum value of i
 * returns 0 if no error was detected else 1
 */
int get_next_mask(FILE *masks_file, Mask *mask, int max_width, int max_height) {
	int max_number_lenght = get_max_number_lenght(max_width < max_height ? max_height:max_width); // Max
	char *temp_string = malloc(sizeof(char) * max_number_lenght + 1); // +1 because null must be at the end of the buffer
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
		temp_string[offset] = 0; // Null at the end
		
		int temp_int = atoi(temp_string);

		switch(i) {
			case 0: if (temp_int < 0) return 1; mask->start_i = temp_int; break;
			case 1: if (temp_int < 0) return 1; mask->start_j = temp_int; break;
			case 2: if (temp_int > max_height) return 1; mask->end_i = temp_int; break;
			case 3: if (temp_int > max_width) return 1; mask->end_j = temp_int; break;
		}
	}

	if (i < 4) {
		return 1;
	}

	return 0;
}


/*
 * This function returns an array containing all the masks of the specified file and
 * at the same time updates the mask_number variable.
 *
 * masks_file_path - the path of the file containing the masks
 * mask_number - the number of masks(to update)
 * max_width - the maximum value of j
 * max_height - the maximum value of i
 * returns an array containing all the masks
 */
Mask* get_all_masks(char *masks_file_path, int *masks_number, int max_width, int max_height) {
	Mask *masks = malloc(sizeof(Mask));
	*masks_number = 0;
	FILE *masks_file = fopen(masks_file_path, "r");

	while (get_next_mask(masks_file, &masks[*masks_number], max_width, max_height) == 0) {
		*masks_number = *masks_number + 1;
		masks = realloc(masks, sizeof(Mask) * (*masks_number + 1));
	}

	fclose(masks_file);
	return masks;
}


/*
 * This function verifies if the specified pixel is in at least one of the specified masks
 *
 * mask_number - the number of masks
 * masks - the list containing the masks
 * i - the row of the pixel
 * j - the column of the pixel
 * returns true if the specified pixel is in at least one of the specified masks else false
 */
bool is_pixel_in_masks(int masks_number, Mask* masks, int i, int j) {
	for (int mask = 0; mask < masks_number; ++mask) {
		if (masks[mask].start_i <= i && i <= masks[mask].end_i && masks[mask].start_j <= j && j <= masks[mask].end_j) {
			return true;
		}
	}
	return false;
}


/*
 * This function determines the number of digits needed to represent the specified integer in base 10
 *
 * i - the integer
 * returns the number of digits needed to represent the specified integer in base 10
 */
int get_max_number_lenght(int i) {
	if (i < 10) {
		return 1;
	}
	return 1 + get_max_number_lenght(i/10);
}
