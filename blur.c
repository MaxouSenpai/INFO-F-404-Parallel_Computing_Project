#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "blur.h"
#include "image.h"
#include "mask.h"

#define WIDTH 1280
#define HEIGHT 720

// https://rawpixels.net/

MPI_Datatype mpi_mask_type;

void blur_host(int world_size) {
	Image image;
	load_raw_image("data/police1.raw", WIDTH, HEIGHT, &image);

	int masks_number;
	Mask* masks = get_all_masks("data/mask1", &masks_number);

	// masks_number = 1; // TODO

	for (int i = 0; i < masks_number; ++i) {
		printf("Mask %i : %i, %i, %i, %i\n", i, masks[i].start_i, masks[i].start_j, masks[i].end_i, masks[i].end_j);
	}

	/*for (int i = 0; i < *masks_number; ++i) {
		printf("%i %i %i %i\n", masks[i]->start_i, masks[i]->start_j, masks[i]->end_i, masks[i]->end_j);
	}*/

	int *distributed_height = calloc(0, sizeof(int) * (world_size - 1));

	int lines = 0;
	while (lines < HEIGHT) {
		for (int j = 0; j < world_size - 1 && lines < HEIGHT; ++j) {
			distributed_height[j]++;
			lines++;
		}
	}

	for (int i = 0; i < world_size-1; ++i) {
		printf("Processor : %i\n", distributed_height[i]);
	}

	// MPI_UNSIGNED_CHAR

	int k = 4;

	MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	MPI_Bcast(&image.width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image.height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(image.data, image.width * image.height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(&masks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(masks, masks_number, mpi_mask_type, 0, MPI_COMM_WORLD);

	/*for (int i = 0; i < masks_number; ++i) {
		MPI_Bcast(masks[i], 4, MPI_INT, 0, MPI_COMM_WORLD);
	}*/

	int current = 0;
	for (int i = 1; i < world_size; ++i) {
		MPI_Send(&current, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Start
		current = current + distributed_height[i-1];
		MPI_Send(&current, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // End
	}

	unsigned char *out_image = malloc(sizeof(unsigned char) * image.width * image.height);

	int delta_data = 0;
	for (int i = 1; i < world_size; ++i) {
		MPI_Recv(out_image + delta_data, image.width * distributed_height[i-1], MPI_UNSIGNED_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		delta_data = delta_data + distributed_height[i-1] * image.width;
	}

	FILE *out_file = fopen("test.raw", "wr");
	fwrite(out_image, 1, image.width * image.height, out_file);
	fclose(out_file);
}

void blur_worker() {
	int k;
	MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

	Image image;

	MPI_Bcast(&image.width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image.height, 1, MPI_INT, 0, MPI_COMM_WORLD);

	image.data = malloc(image.width * image.height * sizeof(unsigned char));
	
	MPI_Bcast(image.data, image.width * image.height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	int masks_number;
	MPI_Bcast(&masks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);

	Mask *masks = malloc(sizeof(Mask) * masks_number);
	MPI_Bcast(masks, masks_number, mpi_mask_type, 0, MPI_COMM_WORLD);

	int start, end;

	MPI_Recv(&start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(&end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);


	Image cut_image;
	cut_image.width = image.width;
	cut_image.height = end - start;
	cut_image.data = malloc(sizeof(unsigned char) * cut_image.width * cut_image.height);


	for (int i = start; i < end; ++i) {
		for (int j = 0; j < image.width; ++j) {
			unsigned char pixel;
			if (is_pixel_in_masks(masks_number, masks, i, j)) {
				pixel = get_blur_pixel(&image, i, j, k);
			}
			else {
				pixel = get_pixel(&image, i, j);
			}

			set_pixel(&cut_image, i - start, j, pixel);
		}
	}
	MPI_Send(cut_image.data, cut_image.width * cut_image.height, MPI_UNSIGNED_CHAR, 0, 0, MPI_COMM_WORLD);
}

void blur(int rank, int world_size) {
	if (rank == 0) {
		blur_host(world_size);
	}
	else {
		blur_worker();
	}
}

int main(int argc, char **argv) {
	int rank, world_size;
	MPI_Init(&argc, &argv);
	init_type();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	blur(rank, world_size);

	printf("Before Finalize\n");
	MPI_Finalize();
	printf("After Finalize\n");
	return 0;
}

void init_type() {
	const int nitems = 4;
	int block_lenghts[4] = {1,1,1,1};
	MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Aint offsets[4];
	offsets[0] = offsetof(Mask, start_i);
	offsets[1] = offsetof(Mask, start_j);
	offsets[2] = offsetof(Mask, end_i);
	offsets[3] = offsetof(Mask, end_j);
	MPI_Type_create_struct(nitems, block_lenghts, offsets, types, &mpi_mask_type);
	MPI_Type_commit(&mpi_mask_type);
}
