// Cosor Mihai, 312CA

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "header.h"

// In aceasta sursa se afla functiile necesare executiilor
// comenzilor suportate de program

#define DELIMITERS "\n "
#define INPUT_SIZE_LINE 1000

// Functia seteaza parametrii structurii pentru coordonate
// la dimensiunile imaginii

void select_all(image_format *image, axis *coordinates,
				int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else {
		coordinates->x0 = 0;
		coordinates->y0 = 0;
		coordinates->x1 = image->width;
		coordinates->y1 = image->height;

		printf("Selected ALL\n");
	}
}

// Functia verifica introducerea corecta a parametrilor comenzii "SELECT",
// verifica daca acestia sunt valizi iar in cazul favorabil seteaza
// coordonatele selectiei la valorile introduse

void select_operation(char *token, image_format *image,
					  axis *coordinates, int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");
		return;
	}
	token = strtok(NULL, DELIMITERS);
	int points[4];

	for (int i = 0; i < 4; i++)
		points[i] = 0;

	if (!token) {
		printf("Invalid command\n");
	} else if (strcmp(token, "ALL") == 0) {
		select_all(image, coordinates, check_image_loaded);
	} else {
		// Acest loop verifica daca s-au introdus toti parametrii, respectiv
		// daca acestia respecta formatul si sunt numere intregi

		for (int i = 0; i < 4; i++) {
			if (!token) {
				printf("Invalid command\n");
				return;
			}
			if (token[0] != '-') {
				for (unsigned long j = 0; j < strlen(token); j++) {
					if (isdigit(token[j]) == 0) {
						printf("Invalid command\n");
						return;
					}
				}
				points[i] = atoi(token);
				token = strtok(NULL, DELIMITERS);
			} else {
				points[i] = atoi(token);
				token = strtok(NULL, DELIMITERS);
			}
		}

		// Se asigura functionarea si in cazul introducerii intr-o
		// ordine eronata a parametrilor

		if (points[0] > points[2]) {
			int aux1 = points[0];
			points[0] = points[2];
			points[2] = aux1;
		}

		if (points[1] > points[3]) {
			int aux2 = points[1];
			points[1] = points[3];
			points[3] = aux2;
		}

		// Se face verificarea ca parametrii sa determine o arie nenula
		// si care se incadreaza in limitele imaginii

		if (points[0] == points[2] || points[1] == points[3] ||
			points[0] < 0 || points[1] < 0 || points[2] < 0 ||
			points[3] < 0 || points[0] > image->width ||
			points[1] > image->height || points[2] > image->width ||
			points[3] > image->height || (points[0] == 0 &&
			points[1] == 0 && points[2] == 0 && points[3] == 0) ||
			(points[0] == points[1] && points[1] == points[2] &&
			points[2] == points[3])) {
			printf("Invalid set of coordinates\n");

		} else {
			coordinates->x0 = points[0];
			coordinates->y0 = points[1];
			coordinates->x1 = points[2];
			coordinates->y1 = points[3];
			printf("Selected %d %d %d %d\n", points[0],
				   points[1], points[2], points[3]);
		}
	}
}

// Functia aplica filtrul grayscale folosind formula data,
// si face aproximarea la intreg pentru fiecare pixel

void grayscale_operation(image_format *image, colors *color,
						 axis *coordinates, int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		for (int i = coordinates->y0; i < coordinates->y1; i++) {
			for (int j = coordinates->x0; j < coordinates->x1; j++) {
				double aux = (color->red[i][j] + color->green[i][j] +
							  color->blue[i][j]) / 3;

				color->red[i][j] = round(aux);
				color->green[i][j] = round(aux);
				color->blue[i][j] = round(aux);
			}
		}
		printf("Grayscale filter applied\n");

	} else {
		printf("Grayscale filter not available\n");
	}
}

// Functia aplica filtrul sepia dupa formula data, si de asemenea
// aproximeaza la intreg fiecare pixel

void sepia_operation(image_format *image, colors *color,
					 axis *coordinates, int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		for (int i = coordinates->y0; i < coordinates->y1; i++) {
			for (int j = coordinates->x0; j < coordinates->x1; j++) {
				double aux[3];

				aux[0] = 0.393 * color->red[i][j] + 0.769 * color->green[i][j]
						 + 0.189 * color->blue[i][j];
				aux[1] = 0.349 * color->red[i][j] + 0.686 * color->green[i][j]
						 + 0.168 * color->blue[i][j];
				aux[2] = 0.272 * color->red[i][j] + 0.534 * color->green[i][j]
						 + 0.131 * color->blue[i][j];

				// Daca un pixel sare de valoarea maxima admisa, acesta
				// devine valoarea maxima

				for (int k = 0; k < 3; k++) {
					if (aux[k] > image->scale)
						aux[k] = image->scale;
				}

				color->red[i][j] = round(aux[0]);
				color->green[i][j] = round(aux[1]);
				color->blue[i][j] = round(aux[2]);
			}
		}
		printf("Sepia filter applied\n");

	} else {
		printf("Sepia filter not available\n");
	}
}

// Functia decupeaza sectiunea primita si o salveaza ca noua imagine

void crop_operation(image_format *image, colors *color,
					axis *coordinates, int *check_image_loaded)
{
	int new_height = coordinates->y1 - coordinates->y0;
	int new_width = coordinates->x1 - coordinates->x0;

	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else {
		if (image->image_type[1] == '3' || image->image_type[1] == '6') {
			colors aux_color;
			// Decuparea pentru imagini colore

			alloc_matrix_color(&aux_color, new_height, new_width);

			// Stochez in copii selectia decupata

			for (int i = 0; i < new_height; i++) {
				for (int j = 0; j < new_width; j++) {
					aux_color.red[i][j] = color->red[i +
										  coordinates->y0][j + coordinates->x0];
					aux_color.green[i][j] = color->green[i + coordinates->y0][j
											+ coordinates->x0];
					aux_color.blue[i][j] = color->blue[i + coordinates->y0][j
										   + coordinates->x0];
				}
			}

			// Eliberez memoria matricilor originale, si aloc din nou imagine
			// conform noilor dimensiuni

			free_memory(image, color);
			alloc_matrix_color(color, new_height, new_width);

			for (int i = 0; i < new_height; i++) {
				for (int j = 0; j < new_width; j++) {
					color->red[i][j] = aux_color.red[i][j];
					color->green[i][j] = aux_color.green[i][j];
					color->blue[i][j] = aux_color.blue[i][j];
				}
			}

			free_aux_color(&aux_color, new_height);
		} else {
			// Decuparea pentru imagini noncolore, aceeasi pasi ca mai sus

			struct image aux_image;

			alloc_matrix_noncolor(&aux_image, new_height, new_width);

			for (int i = 0; i < new_height; i++) {
				for (int j = 0; j < new_width; j++)
					aux_image.matrix[i][j] = image->matrix[i +
									   coordinates->y0][j + coordinates->x0];
			}

			free_memory(image, color);
			alloc_matrix_noncolor(image, new_height, new_width);

			for (int i = 0; i < new_height; i++) {
				for (int j = 0; j < new_width; j++)
					image->matrix[i][j] = aux_image.matrix[i][j];
			}

			free_aux(&aux_image, new_height);
		}

		// Modific noile dimensiuni ale imaginii, si o selectez in intregime

		image->height = new_height;
		image->width = new_width;

		coordinates->x0 = 0;
		coordinates->y0 = 0;
		coordinates->x1 = image->width;
		coordinates->y1 = image->height;

		printf("Image cropped\n");
	}
}

// Functia roteste toata imaginea la un unghi de 90 de grade spre dreapta

void rotate_all_90_degrees(image_format *image, colors *color,
						   axis *coordinates)
{
	int new_height = image->width;
	int new_width = image->height;

	// Rotirea pentru imagini colore

	if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		colors aux_color;

		// Aloc memorie unor copii "colors"

		alloc_matrix_color(&aux_color, new_height, new_width);

		// Stochez in copii imaginea rotita la 90 de grade spre dreapta

		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				aux_color.red[i][j] = color->red[new_width - 1 - j][i];
				aux_color.green[i][j] = color->green[new_width - 1 - j][i];
				aux_color.blue[i][j] = color->blue[new_width - 1 - j][i];
			}
		}

		// Eliberez memoria matricilor originale, si aloc din nou imagine
		// conform noilor dimensiuni

		free_memory(image, color);
		alloc_matrix_color(color, new_height, new_width);

		// In imaginea originala adaug imaginea deja rotita

		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++) {
				color->red[i][j] = aux_color.red[i][j];
				color->green[i][j] = aux_color.green[i][j];
				color->blue[i][j] = aux_color.blue[i][j];
			}
		}

		// Eliberez memoria copiilor

		free_aux_color(&aux_color, new_height);
	} else {
		// Aceiasi pasi ca mai sus si pentru imaginea noncolora

		struct image aux_image;

		alloc_matrix_noncolor(&aux_image, new_height, new_width);

		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++)
				aux_image.matrix[i][j] = image->matrix[new_width - 1 - j][i];
		}

		free_memory(image, color);
		alloc_matrix_noncolor(image, new_height, new_width);

		for (int i = 0; i < new_height; i++) {
			for (int j = 0; j < new_width; j++)
				image->matrix[i][j] = aux_image.matrix[i][j];
		}

		free_aux(&aux_image, new_height);
	}

	// Modific noile dimensiuni ale imaginii, si o selectez in intregime

	image->height = new_height;
	image->width = new_width;

	coordinates->x0 = 0;
	coordinates->y0 = 0;
	coordinates->x1 = image->width;
	coordinates->y1 = image->height;
}

// Functia roteste o selectie patrata la un unghi de 90 de grade spre dreapta

void rotate_90_degrees(image_format *image, colors *color, axis *coordinates)
{
	int new_dim = coordinates->y1 - coordinates->y0;

	// Rotirea pentru imagini colore

	if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		colors aux_color;

		// Aloc memorie unor copii "colors"

		alloc_matrix_color(&aux_color, new_dim, new_dim);

		// Stochez in copii selectia rotita la 90 de grade spre dreapta

		for (int i = 0; i < new_dim; i++) {
			for (int j = 0; j < new_dim; j++) {
				aux_color.red[i][j] = color->red[i +
									  coordinates->y0][j + coordinates->x0];
				aux_color.green[i][j] = color->green[i +
										coordinates->y0][j + coordinates->x0];
				aux_color.blue[i][j] = color->blue[i +
									   coordinates->y0][j + coordinates->x0];
			}
		}

		// In imaginea originala adaug selectia din copii deja rotita

		for (int i = 0; i < new_dim; i++) {
			for (int j = 0; j < new_dim; j++) {
				color->red[i + coordinates->y0][j + coordinates->x0] =
				aux_color.red[new_dim - 1 - j][i];

				color->green[i + coordinates->y0][j + coordinates->x0] =
				aux_color.green[new_dim - 1 - j][i];

				color->blue[i + coordinates->y0][j + coordinates->x0] =
				aux_color.blue[new_dim - 1 - j][i];
			}
		}

		// Eliberez memoria copiilor

	free_aux_color(&aux_color, new_dim);
	} else {
		// Rotirea unei imagini noncolore

		struct image aux_image;

		// Aloc memorie unei copii

		alloc_matrix_noncolor(&aux_image, new_dim, new_dim);

		// Stochez in copie selectia rotita la 90 de grade spre dreapta

		for (int i = 0; i < new_dim; i++) {
			for (int j = 0; j < new_dim; j++)
				aux_image.matrix[i][j] = image->matrix[i + coordinates->y0][j +
								   coordinates->x0];
		}

		// In imaginea originala adaug selectia din copie deja rotita

		for (int i = 0; i < new_dim; i++) {
			for (int j = 0; j < new_dim; j++)
				image->matrix[i + coordinates->y0][j + coordinates->x0] =
				aux_image.matrix[new_dim - 1 - j][i];
		}

		// Eliberez memoria copiei

		free_aux(&aux_image, new_dim);
	}
}

// Functia identifica parametrul dat comenzii "ROTATE", verifica
// daca acesta este valid si  daca selectia este patrata sau toata imaginea si
// apeleaza functiile corespunzatoare de numarul de ori necesar

void rotate_operation(char *token, image_format *image, colors *color,
					  axis *coordinates, int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else {
		token = strtok(NULL, DELIMITERS);

		if (!token) {
			printf("Invalid command\n");

		} else if (coordinates->x0 == 0 && coordinates->y0 == 0 &&
				   coordinates->x1 == image->width &&
				   coordinates->y1 == image->height) {
			if (strcmp(token, "0") == 0 || strcmp(token, "360") == 0 ||
				strcmp(token, "-360") == 0) {
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "90") == 0 ||
				strcmp(token, "-270") == 0) {
				rotate_all_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "180") == 0 ||
				strcmp(token, "-180") == 0) {
				rotate_all_90_degrees(image, color, coordinates);
				rotate_all_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "270") == 0 ||
				strcmp(token, "-90") == 0) {
				rotate_all_90_degrees(image, color, coordinates);
				rotate_all_90_degrees(image, color, coordinates);
				rotate_all_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else {
				printf("Unsupported rotation angle\n");
			}
		} else if (coordinates->x1 - coordinates->x0 ==
				   coordinates->y1 - coordinates->y0) {
			if (strcmp(token, "0") == 0 || strcmp(token, "360") == 0 ||
				strcmp(token, "-360") == 0) {
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "90") == 0 ||
				strcmp(token, "-270") == 0) {
				rotate_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "180") == 0 ||
				strcmp(token, "-180") == 0) {
				rotate_90_degrees(image, color, coordinates);
				rotate_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else if (strcmp(token, "270") == 0 ||
				strcmp(token, "-90") == 0) {
				rotate_90_degrees(image, color, coordinates);
				rotate_90_degrees(image, color, coordinates);
				rotate_90_degrees(image, color, coordinates);
				printf("Rotated %s\n", token);

			} else {
				printf("Unsupported rotation angle\n");
			}

		} else {
			printf("The selection must be square\n");
		}
	}
}

// Functia identifica comanda, si apeleaza functia corespunzatoare ei

void operation_check(char *line, image_format *image, colors *color,
					 axis *coordinates, int *check_image_loaded)
{
	char *token, *token_copy;

	token = strdup(line);
	token_copy = token;
	token = strtok(token, DELIMITERS);

	if (strcmp(token, "LOAD") == 0)

		load_operation(token, image, color, coordinates, check_image_loaded);

	else if (strcmp(token, "SAVE") == 0)

		save_operation(token, image, color, check_image_loaded);

	else if (strcmp(token, "SELECT") == 0)

		select_operation(token, image, coordinates, check_image_loaded);

	else if (strcmp(token, "ROTATE") == 0)

		rotate_operation(token, image, color, coordinates, check_image_loaded);

	else if (strcmp(token, "CROP") == 0)

		crop_operation(image, color, coordinates, check_image_loaded);

	else if (strcmp(token, "GRAYSCALE") == 0)

		grayscale_operation(image, color, coordinates, check_image_loaded);

	else if (strcmp(token, "SEPIA") == 0)

		sepia_operation(image, color, coordinates, check_image_loaded);

	else

		printf("Invalid command\n");

	// Elibereaza memoria alocata sirului cu "strdup"

	free(token_copy);
}
