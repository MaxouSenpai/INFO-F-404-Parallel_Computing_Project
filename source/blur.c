#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"
#include "blur.h"
#include "image.h"
#include "mask.h"
#include "options.h"


/*
 * This function executes the code of the host.
 *
 * world_size - the number of processes
 * options - the options of the command line
 */
void blur_host(int world_size, Options options) {
	Image image;
	load_raw_image(&image, options.image_filename, options.width, options.height);

	Mask* masks;
	int masks_number = get_all_masks(&masks, options.masks_filename, options.width, options.height);

	int *distributed_height = balance_work(image.height, world_size);

	send_parameters_to_workers(&image, masks_number, masks, options.n, world_size, distributed_height);

	int *recvcounts = malloc(sizeof(int) * world_size);
	int *displs = malloc(sizeof(int) * world_size);
	int offset = 0;
	for (int i = 0; i < world_size; ++i) {
		displs[i] = offset;
		recvcounts[i] = distributed_height[i] * image.width;
		offset += recvcounts[i];
	}


	Image cut_image;
	cut_image.width = image.width;
	cut_image.height = distributed_height[0];
	cut_image.data = malloc(sizeof(unsigned char) * cut_image.width * cut_image.height);

	blur(&image, &cut_image, masks_number, masks, options.n, 0, distributed_height[0]);


	unsigned char *out_image = malloc(sizeof(unsigned char) * image.width * image.height);

	MPI_Gatherv(cut_image.data, cut_image.width * cut_image.height, MPI_UNSIGNED_CHAR,
				out_image, recvcounts, displs, MPI_UNSIGNED_CHAR,
				0, MPI_COMM_WORLD);


	FILE *out_file = fopen(options.output_image_filename, "wb");
	fwrite(out_image, 1, image.width * image.height, out_file);
	fclose(out_file);

	free(image.data);
	free(masks);
	free(distributed_height);
	free(recvcounts);
	free(displs);
	free(cut_image.data);
	free(out_image);
}


/*
 * This function receives the parameters from the host and
 * stores them in the different specified variables.
 * All the arguments are passed by their address in order to update them.
 *
 * image - the image
 * masks_number - the number of masks
 * masks - the masks
 * n - the n (neighbourhood) value for the blurring algorithm
 * world_size - the number of processes
 * distributed_height - the array containing the number of rows each process has to work on
 */
void send_parameters_to_workers(Image *image, int masks_number, Mask* masks, int n, int world_size, int *distributed_height) {	
	MPI_Bcast(&image->width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image->height, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(image->data, image->width * image->height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(&masks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(masks, masks_number, MPI_MASK, 0, MPI_COMM_WORLD);

	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int current = distributed_height[0];
	for (int i = 1; i < world_size; ++i) {
		MPI_Send(&current, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // Start
		current = current + distributed_height[i];
		MPI_Send(&current, 1, MPI_INT, i, 0, MPI_COMM_WORLD); // End
	}
}


/*
 * This function receives the parameters from the host and
 * stores them in the different specified variables.
 * All the arguments are passed by their address in order to update them.
 *
 * image - the image
 * masks_number - the number of masks
 * masks - the masks
 * n - the n (neighbourhood) value for the blurring algorithm
 * start - the row which the cut image starts at
 * end - the row which the cut image ends at
 */
void receive_parameters_from_host(Image *image, int *masks_number, Mask **masks, int *n, int *start, int *end) {
	MPI_Bcast(&image->width, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&image->height, 1, MPI_INT, 0, MPI_COMM_WORLD);

	image->data = malloc(image->width * image->height * sizeof(unsigned char));	
	MPI_Bcast(image->data, image->width * image->height, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	MPI_Bcast(masks_number, 1, MPI_INT, 0, MPI_COMM_WORLD);
	*masks = malloc(sizeof(Mask) * (*masks_number));
	MPI_Bcast(*masks, *masks_number, MPI_MASK, 0, MPI_COMM_WORLD);

	MPI_Bcast(n, 1, MPI_INT, 0, MPI_COMM_WORLD);

	MPI_Recv(start, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	MPI_Recv(end, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}


/*
 * This function executes the code of a worker.
 */
void blur_worker() {
	Image image;
	int masks_number;
	Mask *masks;
	int n;
	int start, end;

	receive_parameters_from_host(&image, &masks_number, &masks, &n, &start, &end);

	Image cut_image;
	cut_image.width = image.width;
	cut_image.height = end - start;
	cut_image.data = malloc(sizeof(unsigned char) * cut_image.width * cut_image.height);

	blur(&image, &cut_image, masks_number, masks, n, start, end);
	MPI_Gatherv(cut_image.data, cut_image.width * cut_image.height, MPI_UNSIGNED_CHAR,
				NULL, NULL, NULL,
				MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);

	free(image.data);
	free(masks);
	free(cut_image.data);
}


/*
 * This function blurs or not, based on the masks,
 * the pixels of the specified part of the base image and
 * stores the result in the specified output image.
 *
 * base_image - the base image
 * cut_image - the output cut image
 * masks_number - the number of masks
 * masks - the masks
 * n - the n (neighbourhood) value for the blurring algorithm
 * start - the row which the cut image starts at
 * end - the row which the cut image ends at
 */
void blur(Image *base_image, Image *cut_image, int masks_number, Mask *masks, int n, int start, int end) {
	for (int i = start; i < end; ++i) {
		for (int j = 0; j < base_image->width; ++j) {
			unsigned char pixel;
			if (is_pixel_in_masks(masks_number, masks, i, j)) {
				pixel = get_blur_pixel(base_image, i, j, n);
			}
			else {
				pixel = get_pixel(base_image, i, j);
			}

			set_pixel(cut_image, i - start, j, pixel);
		}
	}
}


/*
 * This function determines if the process is the host or a worker.
 * The process with the rank 0 is the host and the others are workers.
 *
 * rank - the rank of the current process
 * world_size - the number of processes
 * argc - the number of arguments of the command line
 * argv - the array containing the arguments of the command line
 */
void blur_split(int rank, int world_size, int argc, char **argv) {
	if (rank == 0) {
		Options options;
		parse_arguments(argc, argv, &options);

		blur_host(world_size, options);
	}
	else {
		blur_worker();
	}
}


/*
 * This function executes the code.
 *
 * argc - the number of arguments of the command line
 * argv - the array containing the arguments of the command line
 * returns 0 if the code did not detect any error else 1
 */
int main(int argc, char **argv) {
	int rank, world_size;
	MPI_Init(&argc, &argv);
	create_mpi_mask_type();
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

	blur_split(rank, world_size, argc, argv);

	MPI_Finalize();
	return 0;
}


/*
 * This function creates the new type Mask for MPI.
 *
 */
void create_mpi_mask_type() {
	const int nitems = 4;
	int block_lenghts[4] = {1,1,1,1};
	MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
	MPI_Aint offsets[4];
	offsets[0] = offsetof(Mask, start_i);
	offsets[1] = offsetof(Mask, start_j);
	offsets[2] = offsetof(Mask, end_i);
	offsets[3] = offsetof(Mask, end_j);
	MPI_Type_create_struct(nitems, block_lenghts, offsets, types, &MPI_MASK);
	MPI_Type_commit(&MPI_MASK);
}


/*
 * This function divides equally the work between different workers.
 *
 * work - the quantity of work to do
 * workers_number - the number of workers
 * returns the array containing for each worker its quantity of work to do
 */
int* balance_work(int work, int workers_number) {
	int *distributed_work = calloc(0, sizeof(int) * workers_number);

	int delta = work / workers_number;
	int left = work % workers_number;

	for (int i = 0; i < workers_number; ++i) {
		distributed_work[i] = delta + (i < left ? 1 : 0); // To split the rest
	}

	return distributed_work;
}
