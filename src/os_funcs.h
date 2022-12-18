#ifndef OS_FUNCS_H_
#define OS_FUNCS_H_

#define _GNU_SOURCE

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

/*
calculate the file size
return the size
*/
long long totalfilessize(const char *filename);
/*
calculate available disk space
return the size
*/
long long totaldiskspace(const char *todir);
/*
returns the file type with mask of & S_IFMT
*/
mode_t filetype(const char *filename);
/*
check if fname exist and coincide with suorce_inode than open a new file 
return the handle or -1 in case of error
*/
int opennew(const char *fname, ino_t source_inode);
/*
perform a copy of the specified file to destination direcory
return the size of bytes copied or -1L in case of error
*/
ssize_t filecopy(const char *source, const char *todir);
/*
remove file from disk
return 0 if success or -1 in case of error
*/
int rm(const char *fname);
/*
show the file information on the screen
return true if success false in case of error
*/
bool file_info(const char *filename);
/*
build a vaild path string complete whit '/'
return a string pointer of the builded path
*/
char *buildpath(const char *dirname);
/*
check if the specified name is a valid file name
return true if is valid or false if is not valid
*/
bool isvalidfilename(const char *filename);
/*
returns the i-node number of a file or directory
*/
ino_t inodeof(const char *filename);

extern bool wildcard, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // OS_FUNCS_H_