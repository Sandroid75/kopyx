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
#include <libgen.h>
#include <sys/sendfile.h>
#include <time.h>
#include <string.h>

#include "ui_funcs.h"

/* calculate the file size */
long long totalfilessize(const char *filename);
/* calculate available disk space */
long long totaldiskspace(const char *todir);
/* returns the file type */
mode_t filetype(const char *filename);
/* opne a new file*/
int opennew(const char *fname);
/* perform a copy of the specified file to destination direcory */
ssize_t filecopy(const char *source, const char *todir);
/* remove file from disk */
int rm(const char *fname);
/* show the file information on the screen */
bool file_info(const char *filename);
/* build a vaild path string complete whit '/' */
char *buildpath(const char *dirname);
/* check if the specified name is a valid file name */
bool isvalidfilename(const char *filename);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // OS_FUNCS_H_