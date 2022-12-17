#ifndef OS_FUNCS_H_
#define OS_FUNCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdbool.h>
#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/statvfs.h>
#include <errno.h>

long long totalfilessize(const char *filename);
long long totaldiskspace(const char *todir);
mode_t filetype(const char *filename);
int opennew(const char *fname);
ssize_t filecopy(const char *source, const char *todir);
int rm(const char *fname);
bool file_info(const char *filename);
char *buildpath(const char *dirname);
bool isvalidfilename(const char *filename);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // OS_FUNCS_H_