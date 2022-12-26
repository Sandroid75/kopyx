#include "kopyx.h"

bool kopyx(const char *fromdir) {
    DIR *dir;
    struct dirent *entry;
    char newpath[FILENAME_MAX];
    bool match = false;

    if(!(dir = opendir(fromdir))) {
        fprintf(stderr, "%s: Error opening directory: %s", __func__, fromdir);
        getyval("\nPress a key to continue...");
        puts("");

        return false;
    }

    while((entry = readdir(dir))) {
        switch(entry->d_type) {
            case DT_DIR: // It's a directory.
                if(entry->d_ino == todir_inode || !strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                    continue;
                }
                if(include_subdirs) { //if it has specified to also search in subdirectories
                    sprintf(newpath, "%s%s/", fromdir, entry->d_name);
                    kopyx(newpath);
                }
                break;
            case DT_REG: // This is a regular file.
                if(wildcard) { // pattern contains wildcards like '*', '?', '+', '@', '!'
                    match = (fnmatch(pattern, entry->d_name, FNM_EXTMATCH) == 0); // FNM_EXTMATCH defined with _GNU_SOURCE
                } else { //pattern contains the exact filename
                    match = (strcmp(pattern, entry->d_name) == 0);
                }
                if(match) {
                    sprintf(newpath, "%s%s", fromdir, entry->d_name);
                    dosomething(newpath); //do what user want
                }
                break;
            case DT_BLK: // This is a block device.
            case DT_CHR: // This is a character device.
            case DT_FIFO: // This is a named pipe (FIFO).
            case DT_LNK: // This is a symbolic link.
            case DT_SOCK: // This is a UNIX domain socket.
            case DT_UNKNOWN: // The file type could not be determined.
            default:
                break;
        }
    }

    closedir(dir);

    return true;
}

void dosomething(const char *source) {
    ssize_t bytes;

    found_one++;

    if(info) { //if it specified to show file info
        file_info(source);
    }

    if(find_only) { //if it have specified to search only
        find(source);
    } else if(standardoutput) { //redirect the output to the screen
        showtoscreen(source);
    } else if(diskspace(source, todir)) {
        printf("Coping %s -> %s\n", source, todir);
        bytes = filecopy(source, todir); //copy the file to the destination
        if(bytes == -1L) {
            fprintf(stderr, "Warning: impossible coping %s to %s\n", source, todir);
            getyval("Press any key to continue...");
            puts("");
        } else if(bytes == -2L) {
            printf("\nSkip coping: suource file coincide with destination file!\n");
        } else if(bytes == -3L) {
            printf("\nSkip coping...\n");
        } else {
            totalbytescopied += bytes;
            printf("%ld byte%scopied...\n", bytes, (bytes > 1L) ? "s " : " ");
        }
    }else {
        fprintf(stderr, "\n%s: Unknown error!\n", __func__);
        getyval("Press a key to continue...");
        puts("");
    }

    return;
}
/* 
void doglob(const char *fullpath) {
    glob_t pglob;
    int flags = GLOB_ERR | GLOB_MARK | GLOB_NOSORT;

    switch(glob(fullpath, flags, NULL, &pglob)) {
        case GLOB_NOSPACE:
            fprintf(stderr, "\n%s: error running out of memory!\n", __func__);
            getyval("Press a key to continue...");
            puts("");
            break;
        case GLOB_ABORTED:
            fprintf(stderr, "\n%s: read error!\n", __func__);
            getyval("Press a key to continue...");
            puts("");
            break;
        case GLOB_NOMATCH:
            break;
        default:
            for(int i = 0; i < pglob.gl_pathc; i++) {
                dosomething(pglob.gl_pathv[i]);
            }
            break;
    }
    globfree(&pglob);

    return;
}
 */