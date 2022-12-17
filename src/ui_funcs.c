#include "ui_funcs.h"

int getyval(const char *msg) {
    char entered;

    if(noconfirm) { //if the user specified the -y option
        return true;
    }
    
    fprintf(stderr, "%s", msg);
    entered = getchar();
    if(entered == 0x03) {
        exit(EXIT_FAILURE);
    }
        
    return (tolower(entered) == 'y'); //return true if user input 'Y' or 'y'
}

void arg_error(void) {
    char errmsg[] = 
        "\nKOPYX is a utility that copies and/or deletes from source path\n"
        "and/or sub-dir all files that match the specified filter.\n"
        "If no destination is specified, the current dir will be used.\n"
        "These options can be also specified:\n\n"
        " -d  delete after copying or finding (asks for confirmation)\n"
        " -f  just search (don't copy)\n"
        " -i  show all info of source file\n"
        " -r  also searches subdirectories\n"
        " -v  prompts for confirmation before copying each file\n"
        " -s  redirects the output to the screen (it can also be redirected with \'>\')\n"
        " -y  doesn't ask for confirmation to delete (use with care!)\n\n"
        "Examples:\n\n"
        "kopyx -dr \"*.dat\" /              copies all .dat files from / and all sub-dirs\n"
        "                                   on the current dir ./ and delete the originals.\n\n"
        "kopyx -dy \"*.dat\" /home/ /bkup   copies all .dat files from /home to /bkup\n"
        "                                   directory and delete the source files.\n\n"
        "kopyx -fd \"*.tmp\" /media/        search for all .tmp files in directory /media/\n"
        "                                   than asks for confirmations before deleting.\n\n";

    fprintf(stderr, "%s", errmsg);

    //getyval("\nPress a key to continue...");
    
    exit(EXIT_FAILURE);
}