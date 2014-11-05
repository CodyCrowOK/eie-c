#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "eie.h"

int add(int argc, char **argv)
{
	int i;
	int num_files = argc - 2;
	if (num_files == 0) {
		printf("We need something here.\n");
		print_usage();
		return 1;
	}

	for (i = 2; i < argc; i++) {
		int error = add_file_to_queue(argv[i]);
		if (error) {
			printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
			return 1;
		}
	}
	
	return 0;
}

int add_file_to_queue(char *file)
{
	printf("Added %s\n", file);
	FILE *fp = fopen(eie_addfiles_addr, "a");
	if (!fp) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	fprintf(fp, "%s\n", file);
	fclose(fp);
	return 0;
}

