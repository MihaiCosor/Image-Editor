// Cosor Mihai

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"

// In aceasta sursa se afla functiile necesare citirii din fisiere
// (incluzand evitarea comentariilor) si salvarii in fisiere

#define DELIMITERS "\n "
#define INPUT_SIZE_LINE 1000

// Functia citeste linie cu linie din fisier, cand gaseste
// un comentariu, il ignora

void read_without_comm(FILE *input, image_format *image)
{
	char line[INPUT_SIZE_LINE];

	fgets(line, sizeof(line), input);

	while (line[0] == '#')
		fgets(line, sizeof(line), input);

	int trash = sscanf(line, "%c%c", &image->image_type[0],
					   &image->image_type[1]);

	if (trash != 0)
		trash++;

	fgets(line, sizeof(line), input);

	while (line[0] == '#')
		fgets(line, sizeof(line), input);

	trash = sscanf(line, "%d %d", &image->width, &image->height);

	// Daca imaginea este ascii de tipul grayscale/color, citesc
	// si "scale"-ul acesteia

	if (image->image_type[0] == 'P' &&
	    (image->image_type[1] == '2' || image->image_type[1] == '3')) {
		fgets(line, sizeof(line), input);

		while (line[0] == '#')
			fgets(line, sizeof(line), input);

		trash = sscanf(line, "%lf", &image->scale);
	}

	// Daca imaginea este binara de tipul grayscale/color, citesc
	// si "scale"-ul acesteia

	if (image->image_type[0] == 'P' &&
	    (image->image_type[1] == '5' || image->image_type[1] == '6')) {
		fgets(line, sizeof(line), input);

		while (line[0] == '#')
			fgets(line, sizeof(line), input);
		trash = sscanf(line, "%lf", &image->scale);
	}

	fgets(line, sizeof(line), input);

	while (line[0] == '#')
		fgets(line, sizeof(line), input);

	// Dupa citirea ultimului comentariu, voi citi in "line" si prima linie
	// a matricii, asa ca repozitionez cursorul la inceput de matrice

	fseek(input, -strlen(line), SEEK_CUR);
}

// Functia aloca memorie (indirect) pentru imaginea text, si citeste matricea
// si pentru imaginile colore, cat si pentru cele noncolore

void read_alloc_txt_image(FILE *input, image_format *image, colors *color)
{
	read_without_comm(input, image);

	// Pentru imagini noncolore, folosesc doar structura "image_format"

	if (image->image_type[0] == 'P' &&
	    (image->image_type[1] == '1' || image->image_type[1] == '2')) {
		alloc_matrix_noncolor(image, image->height, image->width);

		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fscanf(input, "%d", &image->matrix[i][j]);
		}
	} else {
		// Pentru imagini colore, folosesc si structura suplimentara "colors"

		alloc_matrix_color(color, image->height, image->width);

		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				fscanf(input, "%lf", &color->red[i][j]);
				fscanf(input, "%lf", &color->green[i][j]);
				fscanf(input, "%lf", &color->blue[i][j]);
			}
		}
	}
}

// Functia aloca memorie (indirect) pentru imaginea binara, si citeste matricea
// si pentru imaginile colore, cat si pentru cele noncolore

void read_alloc_bin_image(FILE *input, image_format *image, colors *color)
{
	read_without_comm(input, image);

	// Pentru imagini noncolore, folosesc doar structura "image_format"

	if (image->image_type[0] == 'P' &&
	    (image->image_type[1] == '4' || image->image_type[1] == '5')) {
		alloc_matrix_noncolor(image, image->height, image->width);

		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				unsigned char aux;
				fread(&aux, sizeof(char), 1, input);
				image->matrix[i][j] = (int)aux;
			}
		}

	} else {
		// Pentru imagini colore, folosesc si structura suplimentara "colors"

		alloc_matrix_color(color, image->height, image->width);

		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				unsigned char aux;

				fread(&aux, sizeof(char), 1, input);
				color->red[i][j] = (double)aux;

				fread(&aux, sizeof(char), 1, input);
				color->green[i][j] = (double)aux;

				fread(&aux, sizeof(char), 1, input);
				color->blue[i][j] = (double)aux;
			}
		}
	}
}

// Functia verifica tipul imaginii prin compararea cifrei de dupa "P"
// pentru a putea deschide ulterior fisierul in modul corespunzator

void load_file_type(char *file_name, char file_type, image_format *image,
					colors *color, int *check_image_loaded)
{
	if (file_type < '4') {
		FILE *input = fopen(file_name, "rt");

		read_alloc_txt_image(input, image, color);
		*check_image_loaded = 1;

		fclose(input);

		// *Comentariu valabil si pentru "if", si pentru "else"*
		// Odata ce s-a incarcat o imagine, "check_image_loaded" devine 1
		// pentru a tine minte ca este o imagine incarcata

	} else {
		FILE *input = fopen(file_name, "rb");

		read_alloc_bin_image(input, image, color);
		*check_image_loaded = 1;

		fclose(input);
	}
}

// Functia verifica daca s-a introdus numele fisierului, daca acesta
// este valid, si il deschide pentru a obtine cifra de dupa "P"

void load_operation(char *token, image_format *image, colors *color,
					axis *coordinates, int *check_image_loaded)
{
	token = strtok(NULL, DELIMITERS);

	char line[INPUT_SIZE_LINE];
	char *file_name = token;
	char file_type;

	if (!file_name) {
		printf("Invalid command\n");

	} else {
		FILE *input = fopen(file_name, "rt");

		// Daca o imagine este deja incarcata, se elibereaza memoria
		// in care aceasta este stocata

		if (*check_image_loaded == 1)
			free_memory(image, color);

		if (!input) {
			printf("Failed to load %s\n", file_name);

			// Daca incarcarea unei imagini esueaza, consideram ca
			// nu mai avem nicio imagine incarcata

			*check_image_loaded = 0;

		} else {
			fgets(line, sizeof(line), input);

			while (line[0] == '#')
				fgets(line, sizeof(line), input);

			file_type = line[1];
			fclose(input);

			load_file_type(file_name, file_type, image, color,
						   check_image_loaded);

			printf("Loaded %s\n", file_name);

			// Odata cu incarcarea imaginii, se face si selectia acesteia

			coordinates->x0 = 0;
			coordinates->y0 = 0;
			coordinates->x1 = image->width;
			coordinates->y1 = image->height;
		}
	}
}

// Functia salveaza informatiile imaginii in format ascii si pentru
// imaginile color, cat si pentru cele noncolor

void save_file_txt(FILE *output, image_format *image, colors *color)
{
	// Schimb cifra de dupa "P" astfel incat sa corespunda
	// formatului text

	if (image->image_type[1] == '4')

		fprintf(output, "%c1\n", image->image_type[0]);

	else if (image->image_type[1] == '5')

		fprintf(output, "%c2\n", image->image_type[0]);

	else if (image->image_type[1] == '6')

		fprintf(output, "%c3\n", image->image_type[0]);

	else

		fprintf(output, "%c%c\n", image->image_type[0], image->image_type[1]);

	fprintf(output, "%d %d\n", image->width, image->height);

	// Daca imaginea este grayscale/color, scriu si "scale"-ul acesteia

	if (image->image_type[1] == '2' || image->image_type[1] == '3' ||
		image->image_type[1] == '5' || image->image_type[1] == '6')

		fprintf(output, "%.0lf\n", image->scale);

	if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fprintf(output, "%.0lf %.0lf %.0lf ", color->red[i][j],
						color->green[i][j], color->blue[i][j]);

			fprintf(output, "\n");
		}
	} else {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++)
				fprintf(output, "%d ", image->matrix[i][j]);

			fprintf(output, "\n");
		}
	}
}

// Functia salveaza informatiile imaginii in format binar si pentru
// imaginile color, cat si pentru cele noncolor

void save_file_bin(FILE *output, image_format *image, colors *color)
{
	// Schimb cifra de dupa "P" astfel incat sa corespunda
	// formatului binar

	if (image->image_type[1] == '1')

		fprintf(output, "%c4\n", image->image_type[0]);

	else if (image->image_type[1] == '2')

		fprintf(output, "%c5\n", image->image_type[0]);

	else if (image->image_type[1] == '3')

		fprintf(output, "%c6\n", image->image_type[0]);

	else

		fprintf(output, "%c%c\n", image->image_type[0], image->image_type[1]);

	fprintf(output, "%d %d\n", image->width, image->height);

	// Daca imaginea este grayscale/color, scriu si "scale"-ul acesteia

	if (image->image_type[1] == '2' || image->image_type[1] == '3' ||
		image->image_type[1] == '5' || image->image_type[1] == '6')

		fprintf(output, "%.0lf\n", image->scale);

	if (image->image_type[1] == '3' || image->image_type[1] == '6') {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				unsigned char aux;

				aux = (char)color->red[i][j];
				fwrite(&aux, sizeof(char), 1, output);

				aux = (char)color->green[i][j];
				fwrite(&aux, sizeof(char), 1, output);

				aux = (char)color->blue[i][j];
				fwrite(&aux, sizeof(char), 1, output);
			}
		}

	} else {
		for (int i = 0; i < image->height; i++) {
			for (int j = 0; j < image->width; j++) {
				unsigned char aux;
				aux = (char)image->matrix[i][j];
				fwrite(&aux, sizeof(char), 1, output);
			}
		}
	}
}

// Functia identifica parametrii dati pentru comanda "SAVE"

void save_operation(char *token, image_format *image, colors *color,
					int *check_image_loaded)
{
	if (*check_image_loaded == 0) {
		printf("No image loaded\n");

	} else {
		token = strtok(NULL, DELIMITERS);

		if (!token) {
			printf("Invalid command\n");

		} else {
			char *file_name = token;
			token = strtok(NULL, DELIMITERS);

			// Este verificat formatul in care trebuie salvata imaginea

			if (!token || strcmp(token, "ascii") != 0) {
				FILE *output = fopen(file_name, "wb");
				save_file_bin(output, image, color);
				fclose(output);

			} else {
				FILE *output = fopen(file_name, "wt");
				save_file_txt(output, image, color);
				fclose(output);
			}

			printf("Saved %s\n", file_name);
		}
	}
}
