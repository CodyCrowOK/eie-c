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
char *eie_dir, *eie_store_dir, *eie_info_dir, *eie_addfiles_addr;

int main(int argc, char **argv)
{
	if (argc < 2) {
		printf("USAGE: eie <command> [arguments]\n");
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
	}

	free(eie_dir);
	free(eie_addfiles_addr);
	free(eie_info_dir);
	free(eie_store_dir);
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
		fclose(file);
		
		return 1;
	} else {
		printf("Cleared.\n");
	}
	return 0;
}

/*
 * Remove an eie repo
 */
int killall()
{
	int status = nftw(eie_dir, delete_file, 2, FTW_CHDIR | FTW_MOUNT | FTW_DEPTH | FTW_ACTIONRETVAL);
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
