#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

#include "eie.h"

char *eie_dir, *eie_store_dir, *eie_info_dir, *eie_addfiles_addr;

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("USAGE: eie <command> [arguments]\n");
		return 0;
	}

	int command, err;
	
	if (argv[1] == "init") command = 0;
	if (argv[1] == "clear") command = 1;

	switch (command) {
	case 0: 
		err = init();
		if (err) printf("init failed\n");
		break;
	case 1:
		err = clear();
		if (err) printf("clear failed\n");
		break;
	}

	free(eie_dir);
}

/*
 * Sets up the directory for usage
 */
int init()
{
	//Generate the eie directory name
	char *cwd = get_current_dir_name();
	eie_dir = malloc(strlen(cwd) + strlen(DIR_NAME) + 3);
	sprintf(eie_dir, "%s/%s", cwd, DIR_NAME);
	
	eie_store_dir = malloc(strlen(eie_dir) + 7);
	sprintf(eie_store_dir, "%s/store", eie_dir);
	
	eie_info_dir = malloc(strlen(eie_dir) + 6);
	sprintf(eie_info_dir, "%s/info", eie_dir);
	
	eie_addfiles_addr = malloc(strlen(eie_dir) + 10);
	sprintf(eie_addfiles_addr, "%s/addfiles", eie_dir);
	
	free(cwd);

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
		return 1;
	}
	

	return 0;
}

/*
 * Just removes the contents of addfiles
 */
int clear()
{
	
	return 0;
}
