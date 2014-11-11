#define _GNU_SOURCE
#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#include "eie.h"

int delete_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

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
		printf("ERROR (nftw) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = rmdir(eie_info_dir);
	if (status == -1) {
		printf("ERROR (rmdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	status = rmdir(eie_store_dir);
	if (status == -1) {
		printf("ERROR (rmdir) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	status = rmdir(eie_dir);
	if (status == -1) {
		printf("ERROR (rmdir) on line %d: %s\n", __LINE__, strerror(errno));
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
