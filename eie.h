#ifndef EIE_H
#define EIE_H 1


#define DIR_NAME ".eie"

char *eie_dir, *eie_store_dir, *eie_info_dir, *eie_addfiles_addr;

void print_usage();
int init();
int clear();

int killall();
int delete_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);

int add(int argc, char **argv);
int add_file_to_queue(char *file);

#endif
