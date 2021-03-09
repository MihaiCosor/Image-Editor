// Cosor Mihai

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "header.h"

// In functia main, citesc comenzile rand cu rand

#define INPUT_SIZE_LINE 1000

int main(void)
{
	char line[INPUT_SIZE_LINE];
	image_format image;
	axis coordinates;
	colors color;
	int check_image_loaded = 0;

	fgets(line, sizeof(line), stdin);

// Citesc cate o linie, cand aceasta este diferita de comanda "EXIT"
// identific ce comanda este cu ajutorul unei functii

	while (strcmp(line, "EXIT") != 0 && strcmp(line, "EXIT\n") != 0) {
		if (strcmp(line, "\n") == 0) {
			fgets(line, sizeof(line), stdin);
			continue;
		}
		operation_check(line, &image, &color, &coordinates,
						&check_image_loaded);
		fgets(line, sizeof(line), stdin);
	}

// Daca citesc comanda "EXIT" verific daca este incarcata vreo imagine
// pentru a elibera memoria, si inchid programul

	if (check_image_loaded == 1)
		free_memory(&image, &color);
	else
		printf("No image loaded\n");

	return 0;
}
