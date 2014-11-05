#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ftw.h>

#include "eie.h"

/*
 * For the purposes of not making the file more difficult to understand,
 * file-scope variables should have the prefix eie_.
 */

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
	}

	free(eie_dir);
	free(eie_addfiles_addr);
	free(eie_info_dir);
	free(eie_store_dir);
	return 0;
}

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

/*
 * Just removes the contents of addfiles
 */
int clear()
{
	FILE *file = fopen(eie_addfiles_addr, "w");
	if (!file) {
		printf("Could not create blank file.\n");		
		return 1;
	} else {
		printf("Cleared.\n");
		fclose(file);
	}
	return 0;
}

/*
 * Remove an eie repo
 */
int killall()
{
	/*
	 * Walk the file tree and delete files, leaving directories
	 */
	int status = nftw(eie_dir, delete_file, 2, FTW_CHDIR | FTW_MOUNT | FTW_DEPTH | FTW_ACTIONRETVAL);

	/*
	 * Remove directories
	 */
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = rmdir(eie_info_dir);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = rmdir(eie_store_dir);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	status = rmdir(eie_dir);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	return 0;
}

/*
 * Called by nftw in killall()
 */
int delete_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf)
{
	sb = sb;
	ftwbuf = ftwbuf;
	//If fpath is a directory, then this is a liar:
	printf("Removing %s\n", fpath);

	//If it's not a file, go to the next entry
	if (tflag != FTW_F) return FTW_CONTINUE;
	
	//If it is, then delete it.
	int status = unlink(fpath);
	if (status == -1) {
		printf("ERROR (mkdir) on line %d: %s\n", __LINE__, strerror(errno));
		return FTW_STOP;
	}

	return FTW_CONTINUE;
}

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

void print_usage()
{
	printf("USAGE: eie <command> [arguments]\n");
	return;
}
