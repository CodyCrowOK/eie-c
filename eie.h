#ifndef EIE_H
#define EIE_H 1


#define DIR_NAME ".eie"

void print_usage();
int init();
int clear();
int killall();
int delete_file(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf);
int add(int argc, char **argv);


#endif
