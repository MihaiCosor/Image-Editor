// Cosor Mihai, 312CA

#pragma once

// Structura pentru informatiile imaginii, cat si pentru matricea noncolor

typedef struct image {
	char image_type[2];
	int width, height;
	double scale;
	int **matrix;

} image_format;

// Structura pentru coordonatele selectiei

typedef struct {
	int x0, x1, y0, y1;

} axis;

// Structura suplimentara pentru matricea imaginilor color

typedef struct {
	double **red, **green, **blue;

} colors;

// Semnaturile functiilor din "alloc_free_resources.c"

void alloc_matrix_noncolor(image_format *image, int height, int width);

void alloc_matrix_color(colors *color, int height, int width);

void free_memory(image_format *image, colors *color);

void free_aux(image_format *aux_image, int new_height);

void free_aux_color(colors *aux_color, int new_height);

// Semnaturile functiilor de "load" din "files_usage.c"

void read_without_comm(FILE *input, image_format *image);

void read_alloc_txt_image(FILE *input, image_format *image, colors *color);

void read_alloc_bin_image(FILE *input, image_format *image, colors *color);

void load_file_type(char *file_name, char file_type, image_format *image,
					colors *color, int *check_image_loaded);

void load_operation(char *token, image_format *image, colors *color,
					axis *coordinates, int *check_image_loaded);

// Semnaturile functiilor din "operations.c"

void select_all(image_format *image, axis *coordinates,
				int *check_image_loaded);

void select_operation(char *token, image_format *image,
					  axis *coordinates, int *check_image_loaded);

void grayscale_operation(image_format *image, colors *color,
						 axis *coordinates, int *check_image_loaded);

void sepia_operation(image_format *image, colors *color,
					 axis *coordinates, int *check_image_loaded);

void crop_operation(image_format *image, colors *color,
					axis *coordinates, int *check_image_loaded);

void rotate_all_90_degrees(image_format *image, colors *color,
						   axis *coordinates);

void rotate_90_degrees(image_format *image, colors *color,
					   axis *coordinates);

void rotate_operation(char *token, image_format *image, colors *color,
					  axis *coordinates, int *check_image_loaded);

void operation_check(char *line, image_format *image, colors *color,
					 axis *coordinates, int *check_image_loaded);

// Semnaturile functiilor de "save" din "files_usage.c"

void save_file_txt(FILE *output, image_format *image, colors *color);

void save_file_bin(FILE *output, image_format *image, colors *color);

void save_operation(char *token, image_format *image, colors *color,
					int *check_image_loaded);
