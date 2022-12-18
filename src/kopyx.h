#ifndef KOPY_H_
#define KOPY_H_

#define _DEFAULT_SOURCE
#include <dirent.h>
#include <glob.h>

#include "ui_funcs.h"
#include "os_funcs.h"

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
glob wildcards check if haystack match with wildcard in needle
return true if match or false if didn't match
thanks to schweikh - Shell glob matcher https://www.ioccc.org/2001/schweikh.c
 */
bool wildcards(const char *haystack, const char *needle);
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
