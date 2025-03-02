#ifndef UI_FUNCS_H_
#define UI_FUNCS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/termios.h>

#ifdef DEBUG
#define DBG_MSG(msg, ...)                                                                                            \
    {                                                                                                                \
        fprintf(stderr, "FILE: %s IN FUNC: %s() LINE: %d : " msg "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__); \
    }
#else
#define DBG_MSG(msg, ...) \
    {                     \
    }
#endif

#define PUTNC(n, c)                   \
    {                                 \
        for (int i = 0; i < n; i++) { \
            putchar(c);               \
        }                             \
    }
#define NSECT 64
#define BYTES 512

/*
checks if the 'Y' or 'y' key was pressed
return true if 'Y' or 'y' was pressed or false for all other keys
*/
int getyval(const char *msg);
/*
check the available disk space to copy the found file
returns true if there is enough space for the copy otherwise false
*/
bool diskspace(const char *source, const char *dest);
/*
shows the contents of the file on the screen
*/
void showtoscreen(const char *from);
/*
show the found file
*/
void find(const char *fname);
/*
delete the file if user choose to delete it
*/
void deletefile(const char *fname);
/*
show the kopyx user guide
*/
void arg_error(void);

extern bool wildcard, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
extern char *pattern, *todir;

#endif // UI_FUNCS_H_