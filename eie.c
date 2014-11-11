#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "eie.h"

int main(int argc, char **argv)
{
	if (argc < 2) {
		print_usage();
		return 0;
	}

	/*
	 * Sadly, all of this needs to be done here, otherwise the
	 * char pointers won't be initialized when they're needed.
	 */
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

	int command, err;
	
	if (strcmp(argv[1], "init") == 0) command = 0;
	if (strcmp(argv[1], "clear") == 0) command = 1;
	if (strcmp(argv[1], "killall") == 0) command = 2;
	//I don't like the command name killall, let's make an alias `destroy`.
	if (strcmp(argv[1], "destroy") == 0) command = 2;
	if (strcmp(argv[1], "add") == 0) command = 3;
	if (strcmp(argv[1], "commit") == 0) command = 4;
	if (strcmp(argv[1], "list") == 0) command = 5;

	switch (command) {
	case 0: 
		err = init();
		if (err) printf("init failed\n");
		break;
	case 1:
		err = clear();
		if (err) printf("clear failed\n");
		break;
	case 2:
		err = killall();
		if (err) printf("killall failed\n");
		break;
	case 3:
		err = add(argc, argv);
		if (err) printf("add failed.\n");
		break;
	case 4:
		err = commit();
		if (err) printf("commit failed.\n");
		break;
	case 5:
		err = list(argc, argv);
		if (err) printf("list failed.\n");
		break;
	default:
		print_usage();
		break;
	}

	free(eie_dir);
	free(eie_addfiles_addr);
	free(eie_info_dir);
	free(eie_store_dir);
	return 0;
}

void print_usage()
{
	printf("USAGE: eie <command> [arguments]\n");
	return;
}
