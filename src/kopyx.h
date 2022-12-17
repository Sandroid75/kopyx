#ifndef KOPY_H_
#define KOPY_H_

//#define _GNU_SOURCE
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <glob.h>
#include <getopt.h>

#include "ui_funcs.h"
#include "os_funcs.h"

#ifdef DEBUG
#define DBG_MSG(msg, ...) { fprintf(stderr, "FILE: %s IN FUNC: %s() LINE: %d : " msg "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); }
#else
#define DBG_MSG(msg, ...) {}
#endif

bool kopyx(const char *fromdir);
void doglob(const char *fullpath);
void dosomething(const char *source);
bool diskspace(const char *source, const char *dest);
void deletefile(const char *fname);
void showtoscreen(const char *from);
void find(const char *fname);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#define NSECT 64
#define BYTES 512

#endif // KOPY_H
