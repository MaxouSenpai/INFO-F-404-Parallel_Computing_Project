#include <stdlib.h>
#include <stdbool.h>

#include "mask.h"


/*
 * This function creates an array containing all the masks of the specified file
 * and returns the number of masks.
 *
 * masks - the pointer to an array of masks
 * masks_file_path - the path of the file containing the masks
 * max_width - the maximum value of j
 * max_height - the maximum value of i
 * returns the number of masks
 */
int get_all_masks(Mask **masks, char *masks_file_path, int max_width, int max_height) {
	*masks = malloc(sizeof(Mask));
	int masks_number = 0;
	Mask current_mask = {-1,-1,-1,-1};
	FILE *masks_file = fopen(masks_file_path, "r");

	if ((*masks) == NULL) {
		printf("Error : cannot add another mask\n");
		return 0;
	}

	if (masks_file == NULL) {
		printf("Error when trying to open the masks file : %s\n", masks_file_path);
		return 0;
	}

	while(fscanf(masks_file, "%u %u %u %u", &current_mask.start_i, &current_mask.start_j, &current_mask.end_i, &current_mask.end_j) != EOF) {
		if (0 <= current_mask.start_i && 0 <= current_mask.start_j && current_mask.end_i < max_height && current_mask.end_j < max_width &&
				current_mask.start_i < current_mask.end_i && current_mask.start_j < current_mask.end_j) {
			masks_number++;
			void *temp = realloc(*masks, sizeof(Mask) * masks_number);
			if (temp == NULL) {
				printf("Error : cannot add another mask\n");
				return masks_number - 1;
			}
			*masks = temp;
			(*masks)[masks_number - 1] = current_mask;
		}
	}
	return masks_number;
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
