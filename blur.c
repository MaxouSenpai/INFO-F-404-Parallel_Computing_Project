#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "image.h"
#include "mask.h"

#define WIDTH 1280
#define HEIGHT 720

void blur_host(int world_size) {
	// load image
	// unsigned char image[WIDTH * HEIGHT];
	// FILE* file = fopen("data/police1.raw", "rb");
	// fread(image, 1, WIDTH * HEIGHT, file);

	// unsigned char image[HEIGHT][WIDTH];
	/*
	for (int i = 0; i < HEIGHT; ++i) {
		fread(image[i], 1, WIDTH, file);
	}

	unsigned char a = image[20][50];
	printf("%i\n", (int)a);
	*/
	Image *image = load_raw_image("data/police1.raw", WIDTH, HEIGHT);
	// printf("Image info : width : %i, height :%i\nPixel at 20,50 : %i\n", image->width, image->height, (int) get_pixel(image, 20, 50));
	printf("Hey before\n");

	/*for (int i = 0; i < HEIGHT; ++i) {
		printf("Hey middle\n");
		for (int j = 0; j < WIDTH; ++i) {
			get_pixel(image, i, j);
		}
	}*/

	printf("Hey after\n");

	// FILE *masks_file = fopen("data/mask1", "r");
	// Mask *current_mask = get_next_mask(masks_file);
	int masks_number;
	Mask** masks = get_all_masks("data/mask1", &masks_number);

	/*for (int i = 0; i < *masks_number; ++i) {
		printf("%i %i %i %i\n", masks[i]->start_i, masks[i]->start_j, masks[i]->end_i, masks[i]->end_j);
	}*/

	int *distributed_height = calloc(0, sizeof(int) * world_size);

	for (int i = 0; i < HEIGHT; ++i) {
		int j = 0;
		for (; j < world_size && i+j < HEIGHT; ++j) {
			distributed_height[j]++;
		}
		i = i + j;
	}

	/*for (int i = 0; i < world_size; ++i) {
		printf("Processor : %i\n", distributed_height[i]);
	}*/

	// MPI_UNSIGNED_CHAR

	int k = 4;

	MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image->width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image->height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	// MPI_Bcast(&image->data, image->width * image->height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(image->data, image->width * image->height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	for (int i = 1; i < world_size; ++i) {
		/* code */
	}

}

void blur_worker() {
	int k;
	MPI_Bcast(&k, 1, MPI_INT, 0, MPI_COMM_WORLD);

	Image *image = malloc(sizeof(Image));

	// base_image->width = WIDTH;
	// base_image->height = HEIGHT;
	MPI_Bcast(&image->width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image->height, 1, MPI_INT, 0, MPI_COMM_WORLD);

	image->data = malloc(image->width * image->height * sizeof(unsigned char));
	
	MPI_Bcast(image->data, image->width * image->height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
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
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	blur(rank, world_size);

	MPI_Finalize();
	return 0;
}
