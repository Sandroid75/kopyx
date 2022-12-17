#ifndef UI_FUNCS_H_
#define UI_FUNCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>

#define PUTNC(n, c) { for(int i = 0; i < n; i++) { putchar(c); } }
#define NSECT 64
#define BYTES 512

/* checks if the 'Y' or 'y' key was pressed */
int getyval(const char *msg);
/* check the available disk space to copy the found file */
bool diskspace(const char *source, const char *dest);
/* shows the contents of the file on the screen */
void showtoscreen(const char *from);
/* show the found file */
void find(const char *fname);
/* delete the file if user choose to delete it */
void deletefile(const char *fname);
/* show the kopyx user guide */
void arg_error(void);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // UI_FUNCS_H_