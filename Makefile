build: ex

ex: main.c operations.c alloc_free_resources.c files_usage.c
	gcc -Wall -Wextra main.c operations.c alloc_free_resources.c files_usage.c -o image_editor -lm
	
clean:
	rm -f ex
