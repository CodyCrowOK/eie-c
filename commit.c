#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <openssl/sha.h>
#include <time.h>

#include "eie.h"

int get_files_and_commit();
int commit_file(char *filename);

int commit()
{
	int err = get_files_and_commit();
	if (err) return 1;
	err = clear();
	if (err) {
		printf("After commit, clear failed.");
		//No need to return 1 here, as the commit was still successful.
	}
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
		commit_file(strtok(line, "\n"));
	}
	
	return 0;
}

int commit_file(char *filename)
{
	char buffer[BUFSIZ];
	//filename = strtok(filename, "\n");

	FILE *file = fopen(filename, "r");
	if (!file) {
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

	//Now we convert the hash into hex representation
	char hash_hex[21];
	//If it's stupid and it works, it isn't stupid and still works. *sigh*
	snprintf(hash_hex, 21, "%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x%x", hash[0], hash[1], hash[2], hash[3], hash[4], hash[5], hash[6], hash[7], hash[8], hash[9], hash[10], hash[11], hash[12], hash[13], hash[14], hash[15], hash[16], hash[17], hash[18], hash[19]);

	//Do the rest
	char destination[strlen(eie_store_dir) + (10 * sizeof(char)) + strlen(hash_hex) + 3];
	int commit_time = (int) time(NULL);
	sprintf(destination, "%s/%d_%s", eie_store_dir, commit_time, hash_hex);
	printf("Commit to %s\n", destination);

	//Write the file to the storage directory
	FILE *committed_file = fopen(destination, "w");
	if (!committed_file) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}

	file = fopen(filename, "r");
	if (!file) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	for (;;) {
		res = fgets(buffer, BUFSIZ, file);
		if (!res) break;
		fprintf(committed_file, "%s", buffer);
	}

	//Create the info for the commit
	char info_destination[strlen(eie_info_dir) + (10 * sizeof(char)) + 2];
	sprintf(info_destination, "%s/%d", eie_info_dir, commit_time);
	FILE *infofile = fopen(info_destination, "a");
	if (!infofile) {
		printf("ERROR (fopen) on line %d: %s\n", __LINE__, strerror(errno));
		return 1;
	}
	
	fprintf(infofile, "%s,%d_%s\n", filename, commit_time, hash_hex);
	
	fclose(infofile);
	fclose(file);
	fclose(committed_file);
	return 0;
}
