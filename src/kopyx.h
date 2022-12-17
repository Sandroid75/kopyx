#ifndef KOPY_H_
#define KOPY_H_

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <glob.h>

#include "ui_funcs.h"
#include "os_funcs.h"

#ifdef DEBUG
#define DBG_MSG(msg, ...) { fprintf(stderr, "FILE: %s IN FUNC: %s() LINE: %d : " msg "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); }
#else
#define DBG_MSG(msg, ...) {}
#endif

/* look for the file in the specified directory */
bool kopyx(const char *fromdir);
/* perform glob in case of wildcard */
void doglob(const char *fullpath);
/* does what the user specified to do on the found file */
void dosomething(const char *source);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // KOPY_H
