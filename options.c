#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "options.h"


/* Default values for the width and the height of the image */
#define WIDTH 1280
#define HEIGHT 720


/*
 * This function parses the arguments from the command line and
 * updates accordingly the specified Options struct.
 *
 * argc - the number of arguments of the command line
 * argv - the array containing the arguments of the command line
 * options - the Options struct to be updated
 * returns 0 if the parsing did not detect any error else 1
 */
int parse_arguments(int argc, char *argv[], Options *options) {
	int option;
	options->image_filename = 0;
	options->output_image_filename = 0;
	options->masks_filename = 0;
	options->n = -1;
	options->width = WIDTH;
	options->height = HEIGHT;

	while ((option = getopt(argc, argv, "o:n:i:m:d:")) != -1) {
		switch (option) {
			case 'i': options->image_filename = optarg; break;
			case 'm': options->masks_filename = optarg; break;
			case 'n': options->n = atoi(optarg); break;
			case 'o': options->output_image_filename = optarg; break;
			case 'd': options->width = atoi(optarg);
					  if (optind < argc && *argv[optind] != '-') {
					  	options->height = atoi(argv[optind]);
					  	optind++;
					  }
					  break;
			default: break;
		}
	}

	if (options->image_filename == 0 || options->masks_filename == 0 || options->n == -1 || options->output_image_filename == 0) {
		return 1;
	}

	return 0;
}
