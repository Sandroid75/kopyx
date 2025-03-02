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

/* https://stackoverflow.com/questions/42156041/copying-a-huge-file-using-read-write-and-open-apis-in-linux */
#define DEFAULT_CHUNK ((ssize_t) 262144L)

/* filecopy() returns error */
#define FC_ERROR -1L // unable to copy
#define FC_SAMEF -2L // suorce and destination are the same file
#define FC_NOOWD -3L // unable to overwrite the destination
#define FC_USKIP -4L // user skiping overwrite

/* Get file or dir name without path */
#define FILENAME(_path) strrchr(_path, '/') ? (strrchr(_path, '/') + 1) : (_path)

/* Check if not root dir than add / at the end of path */
#define ADDSLASH(_path)                      \
    {                                        \
        if (strcmp(_path, "/") &&            \
            _path[strlen(_path) - 1] != '/') \
            strcat(_path, "/");              \
    }
/* Check if not root dir than del / at the end of path */
#define DELSLASH(_path)                      \
    {                                        \
        if (strcmp(_path, "/") &&            \
            _path[strlen(_path) - 1] == '/') \
            _path[strlen(_path) - 1] = '\0'; \
    }

#define MIN(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })
#define MAX(a, b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })

/* Safe version of free() don't need assign NULL after free */
#ifndef FREE
#define FREE(p)        \
    {                  \
        if ((p))       \
            free((p)); \
        (p) = NULL;    \
    }
#endif

/*
calculate the file size
return the size of the specified file
*/
off_t totalfilesize(const char *filename);
/*
calculate available disk space
return the size
*/
size_t totaldiskspace(const char *todir);
/*
returns the file type with mask of & S_IFMT
*/
mode_t filetype(const char *filename);
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

extern bool wildcard, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;

#endif // OS_FUNCS_H_