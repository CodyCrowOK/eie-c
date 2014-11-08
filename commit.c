#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <openssl/sha.h>

#include "eie.h"

int get_files_and_commit();
int commit_file(char *filename);

int commit()
{
	int err = get_files_and_commit();
	if (err) return 1;
	return 0;
}


/*
 * For future reference:
 * I chose to make this function use multiple disk reads instead of being
 * much more complicated. Make of that what you will.
 */
int get_files_and_commit()
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

	while ((read = getline(&line, &len, file) != -1)) {
		commit_file(line);
	}
	
	return 0;
}

int commit_file(char *filename)
{
	char buffer[BUFSIZ];
	
	FILE *file = fopen(filename, "r");
	if (!file) {
		printf("%s", filename);
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	//First, we need to construct the sha1 hash.
	SHA_CTX ctx;
	SHA1_Init(&ctx);
	char *res;

	for (;;) {
		res = fgets(buffer, BUFSIZ, file);
		if (!res) break;
		SHA1_Update(&ctx, buffer, BUFSIZ);
	}
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1_Final(hash, &ctx);
	printf("%s", hash);
	
	return 0;
}
