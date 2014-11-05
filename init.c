#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>

#include "eie.h"

/*
 * Sets up the directory for usage
 */
int init()
{

	//Create the directory and necessary files...
	int status = mkdir(eie_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = mkdir(eie_store_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = mkdir(eie_info_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	FILE *file = fopen(eie_addfiles_addr, "w");
	if (!file) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		fclose(file);
		return 1;
	} else {
		fclose(file);
	}
	
	printf("Blank eie repository created.\n");

	return 0;
}

