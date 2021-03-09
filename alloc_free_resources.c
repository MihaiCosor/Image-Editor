// Cosor Mihai, 312CA

#include <stdio.h>
#include <stdlib.h>

// In aceasta sursa se afla functiile pentru alocarea dinamica,
// incluzand "programarea defensiva", cat si pentru eliberarea memoriei

#include "header.h"

// Functia aloca memorie pentru matricea din structura "image_format"
// folosita pentru imaginile noncolore

void alloc_matrix_noncolor(image_format *image, int height, int width)
{
	image->matrix = (int **)malloc(height * sizeof(int *));

	if (!image->matrix) {
		free(image->matrix);
		fprintf(stderr, "Problema la alocare\n");
		exit(-1);
	}

	// *Comentariu valabil si pentru urmatoarea functie*
	// Toate functiile de alocare contin "programarea defensiva"
	// prin "if"-urile care dealoca ce s-a memorat, si inchid programul

	for (int i = 0; i < height; i++) {
		image->matrix[i] = (int *)malloc(width * sizeof(int));

		if (!image->matrix[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(image->matrix[j]);

			free(image->matrix);
			fprintf(stderr, "Problema la alocare\n");
			exit(-1);
		}
	}
}

// Functia aloca memorie pentru matricile din structura suplimentara "colors"
// folositoare pentru imaginile color

void alloc_matrix_color(colors *color, int height, int width)
{
	color->red = (double **)malloc(height * sizeof(double *));
	color->green = (double **)malloc(height * sizeof(double *));
	color->blue = (double **)malloc(height * sizeof(double *));

	if (!color->red) {
		free(color->red);
		fprintf(stderr, "Problema la alocare\n");
		exit(-1);
	}
	if (!color->green) {
		free(color->green);
		fprintf(stderr, "Problema la alocare\n");
		exit(-1);
	}
	if (!color->blue) {
		free(color->blue);
		fprintf(stderr, "Problema la alocare\n");
		exit(-1);
	}

	for (int i = 0; i < height; i++) {
		color->red[i] = (double *)malloc(width * sizeof(double));
		color->green[i] = (double *)malloc(width * sizeof(double));
		color->blue[i] = (double *)malloc(width * sizeof(double));

		if (!color->red[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(color->red[j]);

			free(color->red);
			fprintf(stderr, "Problema la alocare\n");
			exit(-1);
		}
		if (!color->green[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(color->green[j]);
			free(color->green);
			fprintf(stderr, "Problema la alocare\n");
			exit(-1);
		}
		if (!color->blue[i]) {
			for (int j = i - 1; j >= 0; j--)
				free(color->blue[j]);
			free(color->blue);
			fprintf(stderr, "Problema la alocare\n");
			exit(-1);
		}
	}
}

// Functia elibereaza memoria alocata, fie matricii din "image_format",
// fie celor din "colors"

void free_memory(image_format *image, colors *color)
{
	// Se verifica tipul imaginii, pentru a sti ce dealocare este necesara

	if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		for (int i = 0; i < image->height; i++) {
			free(color->red[i]);
			free(color->green[i]);
			free(color->blue[i]);
		}

		free(color->red);
		free(color->green);
		free(color->blue);
	} else {
		for (int i = 0; i < image->height; i++)
			free(image->matrix[i]);

		free(image->matrix);
	}
}

// Functia elibereaza memoria aloacata copiei matricii din "image_format"
// ce a fost necesara in functiile "crop" si/sau "rotate"

void free_aux(image_format *aux_image, int new_height)
{
	for (int i = 0; i < new_height; i++)
		free(aux_image->matrix[i]);
	free(aux_image->matrix);
}

// Functia elibereaza memoria alocata copiilor matricilor din "colors"
// ce au fost necesare in functiile "crop" si/sau "rotate"

void free_aux_color(colors *aux_color, int new_height)
{
	for (int i = 0; i < new_height; i++) {
		free(aux_color->red[i]);
		free(aux_color->green[i]);
		free(aux_color->blue[i]);
	}
	free(aux_color->red);
	free(aux_color->green);
	free(aux_color->blue);
}
