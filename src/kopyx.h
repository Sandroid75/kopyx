#ifndef KOPY_H_
#define KOPY_H_

#define _DEFAULT_SOURCE
#include <dirent.h>
#include "ui_funcs.h"
#include "os_funcs.h"
#include <fnmatch.h>

/*
looking for the file in the specified directory
return true if no error found, false in case of error
*/
bool kopyx(const char *fromdir);
/*
does what the user specified to do on the found file
*/
void dosomething(const char *source);
/*
perform glob in case of wildcard
*/
/* void doglob(const char *fullpath); */

extern bool wildcard, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern int found_one;
extern char *pattern, *todir;
extern ino_t todir_inode;
extern ssize_t totalbytescopied;

#endif // KOPY_H
