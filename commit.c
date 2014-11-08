#include <stdio.h>

/*
 * For future reference:
 * I chose to make this function use multiple disk reads instead of being
 * much more complicated. Make of that what you will.
 */
int commit()
{
	int count = 0;
	
	char *line;
	size_t len = 0;
	ssize_t read;
	
	FILE *file = fopen(eie_addfiles_addr, "r");
	if (!file) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	while ((read = getline(&line, &len, file) != -1)) {
		count++;
	}
	fclose(file);
	
	file = fopen(eie_addfiles_addr, "r");

	if (!file) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	char files_to_commit[count][LINE_MAX];
	int i = 0;

	while ((read = getline(&line, &len, file) != -1)) {
		strncpy(*files_to_commit[i++], line, LINE_MAX);
	}
	
	
	return 0;
}
