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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <stdbool.h>
#include <getopt.h>
#include <sys/stat.h>
#include "kopyx.h"

int main(int argc, char *argv[]) {
    char *pattern, *fromdir, *todir;
    int opt;
    mode_t st_mode;

    opterr = true;
    while((opt = getopt(argc, argv, "dfirvsy")) != -1) { //sets all options passed on the command line
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
                    fprintf(stderr, "\n%s invalid option %c\n", argv[0], (char) optopt);
                } else {
                    fprintf(stderr, "\n%s invalid option `\\x%x'\n", argv[0], optopt);
                }
                arg_error();
                break;
            default:
                break;
        }
    }

    if((argc - optind) < 2) { //no arguments were passed
        fprintf(stderr, "\n%s\nSpecify at least one source file or directory!\n", argv[0]);
        arg_error();
    } else if((argc - optind) > 3) { //too many arguments have been specified
        fprintf(stderr, "\n%s\nToo many arguments: specify options, source file, start directory and destination directory.\n", argv[0]);
        arg_error();
    }


    pattern = argv[optind];
    if(!isvalidfilename(pattern)) {
        fprintf(stderr, "\n%s : is not valid file name!\n", pattern);
        exit(EXIT_FAILURE);
    }

    fromdir = buildpath(argv[optind +1]);
    st_mode = filetype(fromdir);
    if(st_mode != S_ISDIR) {
        fprintf(stderr, "\n%s : directory not exist!\n", fromdir);
        exit(EXIT_FAILURE);
    }

    if((argc - optind) == 3) {
        todir = buildpath(argv[optind +2]);
        st_mode = filetype(todir);
        if(st_mode != S_ISDIR) {
            exit(EXIT_FAILURE);
        }
    } else {
        todir = strdup("./");
    }
  
    kopyx(pattern, fromdir, todir); //main search and copy function
    free(todir);

    if(!found_one) { //if the source file was not found
        fprintf(stderr, "No files found\n");
        if(!include_subdirs) { //if not specified to search subdirectories
            fprintf(stderr, " if you use the -r option you can search also the subdirectories of %s\n", fromdir);
            fprintf(stderr, " or you can specify / to search the entire disk.\n");
        }
    }
    
    exit(EXIT_SUCCESS);
}
