#include <stdio.h>
#include <glob.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>

#include "eie.h"

int list(int argc, char **argv)
{
	if (argc < 3) {
		print_usage();
		return 0;
	}
	
	glob_t pglob;
	char pattern[strlen(eie_info_dir) + 3];
	sprintf(pattern, "%s/*", eie_info_dir);

	int err = glob(pattern, 0, NULL, &pglob);
	if (err) {
		printf("Glob failed.\n");
		return 1;
	}

	int num_elements = pglob.gl_pathc;

	int i;
	/*
	 * We're going to use another array that corresponds 1-to-1 with the
	 * array of strings, and this array will be initialized to 0 and each
	 * element will be a flag
	 */
	int flags[num_elements];
	for (i = 0; i < num_elements; i++) {
		flags[i] = 0;
	}
	
	char *line;
	size_t len = 0;
	ssize_t read;
	FILE *file;
	for (i = 0; i < num_elements; i++) {
		file = fopen(pglob.gl_pathv[i], "r");
		if (!file) {
			printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
			return 1;
		}

		while ((read = getline(&line, &len, file) != -1)) {
			if (strcmp(strtok(line, ","), argv[2]) == 0) {
				flags[i] = 1;
			}
		}
		
	}

	int is_in_a_commit = 0;
	printf("Commits containing %s:\n", argv[2]);
	for (i = 0; i < num_elements; i++) {
		if (flags[i]) {
			is_in_a_commit = 1;
			printf("%s\n", basename(pglob.gl_pathv[i]));
		}
	}
	if (!is_in_a_commit) printf("No commits found containing the file: %s\n", argv[2]);
	

	
		
	return 0;
}
