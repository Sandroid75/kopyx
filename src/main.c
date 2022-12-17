#include "kopyx.h"

bool wildcard, found_one, delfile, find_only, verify, standardoutput, info, include_subdirs, noconfirm;
char *pattern, *todir;
ino_t todir_inode;

int main(int argc, char *argv[]) {    
    char *fromdir;
    int opt;
    mode_t st_mode;
    struct stat sb;

    opterr = true;
    wildcard = found_one = delfile = find_only = verify = standardoutput = info = include_subdirs = noconfirm = false;

    while((opt = getopt(argc, argv, "dfirvsy")) != -1) { //sets all options passed on the command line
        switch(opt) {
            case 'd': //delete source file if it has user confirmation (default does not delete source file)
                delfile = true;
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
    if(st_mode != S_IFDIR) {
        fprintf(stderr, "\n%s : directory not exist!\n", fromdir);
        exit(EXIT_FAILURE);
    }

    if((argc - optind) == 3) {
        todir = buildpath(argv[optind +2]);
        st_mode = filetype(todir);
        if(st_mode != S_IFDIR) {
        fprintf(stderr, "\n%s : directory not exist!\n", fromdir);
            exit(EXIT_FAILURE);
        }
    } else {
        todir = strdup("./");
    }
    if(lstat(todir, &sb) == -1) {
            fprintf(stderr, "\n%s : error reading i-node number!\n", todir);
            exit(EXIT_FAILURE);
    }
    todir_inode = (ino_t) sb.st_ino;
  
    kopyx(fromdir); //main search and copy function
    free(todir);

    if(!found_one) { //if the source file was not found
        fprintf(stderr, "No files found\n");
        if(!include_subdirs) { //if not specified to search subdirectories
            fprintf(stderr, " if you use the -r option you can search also the subdirectories of %s\n", fromdir);
            fprintf(stderr, " or you can specify / to search the entire disk.\n");
        }
    }
    free(fromdir);
    
    exit(EXIT_SUCCESS);
}
