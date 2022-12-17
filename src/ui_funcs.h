#ifndef UI_FUNCS_H_
#define UI_FUNCS_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>

#define PUTNC(n, c) { for(int i = 0; i < n; i++) { putchar(c); } }

void arg_error(void);
int getyval(const char *msg);

extern bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;
extern ino_t todir_inode;

#endif // UI_FUNCS_H_