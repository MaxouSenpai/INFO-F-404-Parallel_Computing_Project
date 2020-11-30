typedef struct {
	unsigned char *data;
	int width;
	int height;
} Image;


Image* load_raw_image(char *filepath, int width, int height);

unsigned char get_pixel(Image *image, int i, int j);

void set_pixel(Image *image, int i, int j, unsigned char value);
