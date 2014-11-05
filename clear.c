#include <stdio.h>

#include "eie.h"

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
