#ifndef BLUR_H
#define BLUR_H


#include "mpi.h"
#include "options.h"
#include "image.h"
#include "mask.h"


MPI_Datatype MPI_MASK;


int main(int argc, char **argv);


void blur_split(int rank, int world_size, int argc, char **argv);


void blur_host(int world_size, Options options);


void blur_worker();


void blur(Image *base_image, Image *cut_image, int masks_number, Mask *masks, int n, int start, int end);


void create_mpi_mask_type();


int* balance_work(int work, int workers_number);


void send_parameters_to_worker(Image *image, int masks_number, Mask* masks, int n, int world_size, int *distributed_height);


#endif
