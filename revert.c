#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "eie.h"

static void copy_file(char *destination, char *source);

int revert(int argc, char **argv)
{
	if (argc < 4) {
		print_usage();
		return 0;
	}

	char *oldfile = argv[2];
	char *commit = argv[3];

	char *commitfile = (char *) malloc(strlen(eie_info_dir) + strlen(commit) + 2);
	sprintf(commitfile, "%s/%s", eie_info_dir, commit);
	FILE *infofile = fopen(commitfile, "r");
	if (!infofile) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	char *line;
	size_t len = 0;
	ssize_t read;

	char old_commit_filename[strlen(eie_store_dir) + 33];//
	int is_in_commit = 0;

	while ((read = getline(&line, &len, infofile) != -1)) {
		if (strcmp(strtok(line, ","), oldfile) == 0) {
			is_in_commit = 1;
			sprintf(old_commit_filename, "%s/%s", eie_store_dir,
					strtok(strtok(NULL, ","), "\n"));
			break;
		}
	}
	free(commitfile);
	free(line);
	fclose(infofile);

	if (is_in_commit) {
		printf("Reverting commit %s for file %s\n", commit, oldfile);
		copy_file(oldfile, old_commit_filename);
	} else printf("File not found in commit.\n");

	
	
	return 0;
}

static void copy_file(char *destination, char *source)
{
	/*
	 * Commit the file before we revert it, just in case a mistake was made.
	 */
	int err;
	err = add_file_to_queue(destination);
	if (err) printf("Add before revert failed.\n");
	err = commit();
	if (err) printf("Commit before revert failed.\n");

	FILE *sfp = fopen(source, "r");
	FILE *dfp = fopen(destination, "w");
	if (!sfp) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return;
	}
	if (!dfp) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return;
	}

	char buffer[BUFSIZ], *res;

	for (;;) {
		res = fgets(buffer, BUFSIZ, sfp);
		if (!res) break;
		fprintf(dfp, "%s", buffer);
	}

	fclose(sfp);
	fclose(dfp);
	
}
