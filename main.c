/*********************************************************************************
**                                                                              **
**      Copyleft 1991-2021 by Sandroid75 with GNU General Public License.       **
**                                                                              **
**                                                                              **
**  Redistribution and use in source and binary forms, with or without          **
**  modification, are permitted provided that the following conditions are met: **
**                                                                              **
**   * Redistributions of source code must retain the above copyleft notice,    **
**     this list of conditions and the following disclaimer.                    **
**   * Redistributions in binary form must reproduce the above copyleft         **
**     notice, this list of conditions and the following disclaimer in the      **
**     documentation and/or other materials provided with the distribution.     **
**   * Neither the name of Sandroid75 nor the names of its contributors may     **
**     be used to endorse or promote products derived from this software        **
**     without specific prior written permission.                               **
**                                                                              **
**  THIS SOFTWARE IS PROVIDED BY THE COPYLEFT HOLDERS AND CONTRIBUTORS "AS IS"  **
**  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   **
**  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  **
**  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYLEFT OWNER OR CONTRIBUTORS BE     **
**  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         **
**  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        **
**  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    **
**  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     **
**  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     **
**  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  **
**  POSSIBILITY OF SUCH DAMAGE.                                                 **
**                                                                              **
**  This code was made by Sandroid75 to illustrate various C language powers.   **
**                                                                              **
**  My recommendations for developing robust C code are:                        **
**  - first of all read The C Programming Language: ANSI C Version by K&R (C)   **
**  - after read K&R keep always in mind rules and methods                      **
**  - one of the most characteristics and powers of the C is the pointers       **
**  - pointers are very helpfull and flexibile to write efficient code          **
**  - pointers can be dangerous for stable code if you forget the rules         **
**  - if you use pointers for list of datas don't forget to reserve memory      **
**  - if you use pointers for list of datas don't forget to release memory      **
**  - write well-formatted code                                                 **
**  - only good formatted code makes the code readable                          **
**  - good formatting code reduces the risk of errors                           **
**  - good formatting code facilitates the debugging                            **
**  - good formatting code facilitates the maintenences                         **
**  - good formatting code facilitates future implementantions                  **
**  - commenting the code is another good and necessary practice                **
**  - commenting the code means understanding what you are doing                **
**  - commenting the code means understanding what you have done                **
**  - commenting the code is not a waste of time                                **
**  - at the last but not least, remember rules and methods                     **
**                                                                              **
**                  Have fun with C programming by Sandroid75                   **
**                                                                              **
**********************************************************************************/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <stdbool.h>
#include <getopt.h>
#include "kopyx.h"

int main(int argc, char *argv[]) {
    char *pattern, *ptrstr, *sourcedir, *sourcefile, *destdir;
    int opt, secarg, last;
    mode_t st_mode;

    opterr = true;
    while((opt = getopt(argc, argv, "dfisvry")) != -1) { //sets all options passed on the command line
        switch(opt) {
            case 'd': //delete source file if it has user confirmation (default does not delete source file)
                delete = true;
                break;
            case 'f': //search only (default also copies)
                find_only = true;
                break;
            case 'i': //view all the info of the source file
                info = true;
                break;
            case 'r': //extend the search to the specified subdirectory (default use only the specified directory)
                include_subdirs = true;
                break;
            case 'v': //asks for verification before continuing (default continues - deletion always requires confirmation)
                verify = true;
                break;
            case 's': //redirect the copy to the standard output device, from which it can be redirected using the redirection symbol '>'
                standardoutput = true;
                break;
            case 'y': //does not ask for confirmation before deleting
                noconfirm = true;
                break;
            case '?': //the option entered is not valid
                if(isprint(optopt)) {
                    printf("\n%s invalid option %c\n", argv[0], (char) optopt);
                } else {
                    printf("\n%s invalid option `\\x%x'\n", argv[0], optopt);
                }
                arg_error();
                break;
            default:
                break;
        }
    }

    if(argc <= optind) {//no arguments were passed
        fprintf(stderr, "\n%s\nSpecify at least one source file or directory\n", argv[0]);
        arg_error();
    } else if((argc - optind) > 2) {//too many arguments have been specified
        fprintf(stderr, "\n%s\nToo many arguments specify a source file or directory and possibly a destination directory\n", argv[0]);
        arg_error();
    }

    pattern = strdup(argv[optind]); //duplicates the argument in pattern
    st_mode = filetype(pattern); //assign st_mode a value to check if the argument is a directory or a file (may include the path to the file)
    switch(st_mode) {
        case S_IFREG: //if the argument contains a full path to a file
            if(strcmp(dirname(pattern), "/")) { //verify that the dir is different from the root /
                asprintf(&sourcedir, "%s/", dirname(pattern)); //builds the path by adding the final slash
            } else {
                sourcedir = strdup(dirname(pattern)); //returns only part of the pathname
            }
            sourcefile = strdup(basename(pattern)); //returns only the file name without path
            free(pattern);
            break;
        case S_IFDIR: //if the argument contains a directory WITHOUT a file name
            if(strcmp(pattern, "/")) { //verify that the dir is different from the root /
                for(ptrstr = pattern; *ptrstr; ptrstr++); //increment the ptrstr pointer to the end of the string
                ptrstr--; // now the ptrstr pointer will point to the last character of the pattern string
                //the loop deletes all / characters at the end of the string
                while(ptrstr != pattern && *ptrstr == '/') { //if ptrstr does not point to the beginning of the pattern string, then it is not the directory / and if the last character of the argument is /
                    *ptrstr = '\0'; //then truncate the string by deleting the last character /
                    ptrstr--; //decrements the pointer to the previous character
                }
                asprintf(&sourcedir, "%s/", pattern); //assign the path and add the slash
            } else {
                sourcedir = strdup(pattern); //duplicate the path
            }
            asprintf(&sourcefile, "*"); //a source file has not been specified will copy all the contents of the specified directory *
            free(pattern);
            break;
        case false: //the file does not exist in the specified directory
            if(!include_subdirs) { //if not specified to search subdirectories
                fprintf(stderr, "\nFile \'%s\': does not exist in the specified directory, use -r to search sub-dirs\n", pattern);
                free(pattern);
                exit(1);
            }
            sourcedir = strdup(dirname(pattern)); //returns only part of the pathname
            if(strcmp(sourcedir, "/")) {//verify that the dir is different from the root /
                strcat(sourcedir, "/"); //adds the slash
            }
            sourcefile = strdup(basename(pattern)); //returns only the file name without path
            free(pattern);
            break;
        default:
            fprintf(stderr, "\nSource: \'%s\' invalid\n", pattern);
            free(pattern);
            exit(EXIT_FAILURE);
            break;
    }

    if(sourcedir[0] != '.' && sourcedir[0] != '/') { //if the directory does not begin with. and it doesn't start with / adds a "./" at the beginning of the variable
        asprintf(&pattern, "./%s", sourcedir);
        free(sourcedir);
        sourcedir = strdup(pattern);
        free(pattern);
    }
    
    if(!(argv[optind+1])) {//if not specified the destination defines the current dir
        destdir = strdup("./");
    } else {
        secarg = optind +1; //index of the second argument
        last = strlen(argv[secarg]) -1; //points to the last character before NULL
        
        if(argv[secarg][last] != '/') { //verify that the last character is different from slash
            asprintf(&pattern, "%s/", argv[secarg]); //adds a slash at the end of the path
        } else {
            pattern = strdup(argv[secarg]); //duplicate the path as it is
        }
        
        if(pattern[0] != '.' && pattern[0] != '/') { //verify that the first character is not ne. neither /
            asprintf(&destdir, "./%s", pattern); //inserts the ./ as a path relative to the current location
        } else {
            destdir = strdup(pattern); //duplicate the path as it is
        }
        free(pattern);
    }
    
    copyall(sourcedir, sourcefile, destdir); //main search and copy function

    if(!found_one) { //if the source file was not found
        fprintf(stderr, "No files found\n");
        if(!include_subdirs) { //if not specified to search subdirectories
            fprintf(stderr, " if you use the -r option you can search the subdirectories of %s\n", sourcedir);
            fprintf(stderr, " or you can specify /%s to search the entire disk.\n", sourcefile);
        }
    }
    free(sourcedir);
    free(sourcefile);
    free(destdir);
    
    exit(EXIT_SUCCESS);
}
