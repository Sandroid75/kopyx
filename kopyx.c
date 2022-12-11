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
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/sendfile.h>
#include <errno.h>
#include <dirent.h>
#include <glob.h>
#include "kopyx.h"

bool kopyx(const char *pattern, const char *fromdir, const char *todir) {
    DIR *dir;
    struct dirent *entry;
    char newpath[FILENAME_MAX];

    if(!(dir = opendir(fromdir))) {
        fprintf(stderr, "\n%s: Error opening directory: %s\n", __func__, fromdir);
        getyval("\nPress a key to continue...");

        return false;
    }

    while((entry = readdir(dir))) {
        switch(entry->d_type) {
            case DT_DIR: // It's a directory.
                if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) {
                    continue;
                }
                if(wildcard) {
                    sprintf(newpath, "%s%s", entry->d_name, pattern);
                    doglob(newpath, todir);
                }
                if(include_subdirs) { //if it has specified to also search in subdirectories
                    sprintf(newpath, "%s/%s", fromdir, entry->d_name);
                    kopyx(pattern, newpath, todir);
                }
                break;
            case DT_REG: // This is a regular file.
                if(!wildcard && (strcmp(pattern, entry->d_name) == 0)) {
                    sprintf(newpath, "%s/%s", fromdir, pattern);
                    dosomething(newpath, todir); //do what user want
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

void doglob(const char *fullpath, const char *todir) {
    glob_t *pglob;
    int flags = GLOB_ERR | GLOB_MARK | GLOB_NOSORT;

    switch(glob(fullpath, flags, NULL, pglob)) {
        case GLOB_NOSPACE:
            fprintf(stderr, "\n%s: error running out of memory!\n", __func__);
            getyval("\nPress a key to continue...");
            break;
        case GLOB_ABORTED:
            fprintf(stderr, "\n%s: read error!\n", __func__);
            getyval("\nPress a key to continue...");
            break;
        case GLOB_NOMATCH:
            break;
        default:
            for(int i = 0; i < pglob->gl_pathc; i++) {
                dosomething(pglob->gl_pathv[i], todir);
            }
            break;
    }
    globfree(pglob);

    return;
}

void dosomething(const char *source, const char *dest) {
    found_one = true;

    if(info) { //if it specified to show file info
        file_info(source);
    }

    if(find_only) { //if it have specified to search only
        find(source);
    } else if(standardoutput) { //redirect the output to the screen
        showtoscreen(source);
    } else if(diskspace(source, dest)) {
        filecopy(source, dest); //copy the file to the destination
    }else {
        fprintf(stderr, "\n%s: Unknown error!\n", __func__);
        getyval("\nPress a key to continue...");
    }

    return;
}

long long totaldiskspace(const char *todir) {
    struct statvfs diskinfo;
    
    statvfs(todir, &diskinfo); //get disk attributes

    return (long long) diskinfo.f_bsize * diskinfo.f_bavail; //calculates the available disk space
}

long long totalfilessize(const char *filename) {
    long long filesize;
    FILE *fp = fopen(filename, "r");

    if(fp == NULL) {
        fprintf(stderr, "\n%s: Error opening %s\n", __func__, filename);
        getyval("\nPress a key to continue...");

        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    filesize = (long long) ftell(fp);
    fclose(fp);

    return (long long) filesize;
}

bool diskspace(const char *source, const char *dest) {
    ssize_t totalfilebytes, diskavailbytes;

    totalfilebytes = totalfilessize(source); //calculate file size
    diskavailbytes = totaldiskspace(dest); //calculates the total space on the destination disk

    if(diskavailbytes <= totalfilebytes) {
        fprintf(stderr, "\nInsufficient space on: \n%s\n\nplease free some space before trying again.", dest);
        fprintf(stderr, "Total space required for the copy: %ld bytes\n", totalfilebytes);
        fprintf(stderr, "Space available on the destination drive: %ld bytes\n", diskavailbytes);
        fprintf(stderr, "Minimum space to free before copying: %ld bytes\n", diskavailbytes - totalfilebytes);

        return false;
    }

    return true;
}

void deletefile(const char *fname) {
    if(delete) {
        printf("\nFile: %s", fname);
        if(getyval(" - delete (Yes/No)?")) {
            if(!rm(fname)) {
                printf("deleted!\n");
            }
        }
    }

    return;  
}

void showtoscreen(const char *from) {
    size_t bytes;
    char buffer[NSECT * BYTES];
    FILE *fptr;
    
    if(verify) {
        printf("File: %s", from);
        if(!getyval(" - Show (Yes/No)?")) {
            return;
        }
        printf("\n");
    }

    fptr = fopen(from, "r");
    if(!fptr) {
        fprintf(stderr, "\n%s: Error opening file %s\n", __func__, from);
        getyval("\nPress a key to continue...");
        
        return;
    }

    while((bytes = fread(buffer, sizeof(char), NSECT * BYTES, fptr)) > 0) {
        fwrite(buffer, sizeof(char), bytes, stdout);
    }
    fclose(fptr);
    deletefile(from);
    
    return;
}

ssize_t filecopy(const char *source, const char *destination) {
    int input, output;
    struct stat fileinfo = { 0 };
	ssize_t result = -1L;
    off_t bytesCopied = 0;
	char *errnomsg;

    fprintf(stderr, "\nCoping %s -> %s", source, destination);

    if(verify) { //if the user specified -v
        if(!getyval(" - copy (Yes/No)?")) {

            return result;
        }
    }

    if((input = open(source, O_RDONLY)) == -1) { //try opening the source file
        fprintf(stderr, "\n%s: File open error: %s\n", __func__, source);
        getyval("\nPress a key to continue...");

        return result;
    }

	errno = 0;
    if(fstat(input, &fileinfo)) { //read the attributes of the source file
        switch(errno) {
            case EACCES:
                errnomsg = "Search permission is denied for one of the directories in the path prefix of source file.";
                break;
            case EBADF:
                errnomsg = "fd is not a valid open file descriptor.";
                break;
            case EFAULT:
                errnomsg = "Bad address.";
                break;
            case ELOOP:
                errnomsg = "Too many symbolic links encountered while traversing the path.";
                break;
            case ENAMETOOLONG:
                errnomsg = "Source pathname is too long.";
                break;
            case ENOENT:
                errnomsg = "A component of source pathname does not exist or is a dangling symbolic link.";
                break;
            case ENOMEM:
                errnomsg = "Out of memory (i.e., kernel memory).";
                break;
            case ENOTDIR:
                errnomsg = "A component of the path prefix of pathname is not a directory.";
                break;
            case EOVERFLOW:
                errnomsg = "Source pathname refers to a file whose size, inode number, or number of blocks cannot be represented in, respectively, the types off_t, ino_t, or blkcnt_t.\nThis error can occur when, for example, an application compiled on a 32-bit platform without -D_FILE_OFFSET_BITS=64 calls stat() on a file whose size exceeds (1<<31)-1 bytes.";
                break;
            case 0:
            default:
                errnomsg = NULL;
                break;
        }
        fprintf(stderr, "\n%s: File verification error %s\nerrno: [%d] %s\n", __func__, source, errno, errnomsg);

        return result;
    }

    if((output = opennew(destination)) == -1) { //try to open the target file
        close(input);
        fprintf(stderr, "\n%s: Write error: %s\n", __func__, destination);

        return result;
    }

	errno = 0;
    result = sendfile(output, input, &bytesCopied, fileinfo.st_size); //sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+

	switch(errno) {
		case EAGAIN:
			errnomsg = "Nonblocking I/O has been selected using O_NONBLOCK and the write would block.";
			break;
		case EBADF:
			errnomsg = "The input file was not opened for reading or the output file was not opened for writing.";
			break;
		case EFAULT:
			errnomsg = "Bad address.";
			break;
		case EINVAL:
			errnomsg = "Descriptor is not valid or locked, or an mmap(2)-like operation is not available for in_fd, or count is negative.";
			break;
		case EIO:
			errnomsg = "Unspecified error while reading from in_fd.";
			break;
		case ENOMEM:
			errnomsg = "Insufficient memory to read from in_fd.";
			break;
		case EOVERFLOW:
			errnomsg = "count is too large, the operation would result in exceeding the maximum size of either the input file or the output file.";
			break;
		case ESPIPE:
			errnomsg = "offset is not NULL but the input file is not seekable.";
			break;
		case 0:
		default:
			errnomsg = NULL;
			break;
	}
	if(errno) { //if an error has occurred
		fprintf(stderr, "\n%s: File copy error %s\nerrno: [%d] %s\n", __func__, source, errno, errnomsg); //print the error message on stderr
	}
	if(result != fileinfo.st_size) { //verify that all bytes have been copied
		fprintf(stderr, "\n%s: Error not all data was copied!\nWrited %ld bytes on %ld bytes\n", __func__, result, fileinfo.st_size);
        result = getyval("Continue (Yes/No)?") ? result : -1L; //if the user confirms, the copying of other files continues
	}

    close(input); //close the handle
    close(output); //close the handle
    deletefile(source);

    return result; //return the number of bytes copied
}

int rm(const char *fname) {
    int result;
    char *errnomsg;

    errno = 0;
    result = filetype(fname);
    if(result == S_IFREG) {
        result = remove(fname);
    } else if(result == S_IFDIR) {
        result = rmdir(fname);
    } else {
        fprintf(stderr, "\n%s: Error %s is not a file or directory.\n", __func__, fname);
        exit(EXIT_FAILURE);
    }

    switch(errno) {
		case EACCES:
			errnomsg = "Search permission is denied for a component of the path prefix, or write permission is denied on the directory containing the directory entry to be removed.";
			break;
		case EBUSY:
			errnomsg = "The file named by the path argument cannot be unlinked because it is being used by the system or another process and the implementation considers this an error.";
			break;
		case ELOOP:
			errnomsg = "A loop exists in symbolic links encountered during resolution of the path argument.";
			break;
		case ENAMETOOLONG:
			errnomsg = "The length of a component of a pathname is longer than {NAME_MAX}.";
			break;
		case ENOENT:
			errnomsg = "A component of path does not name an existing file or path is an empty string.";
			break;
		case ENOTDIR:
			errnomsg = "A component of the path prefix names an existing file that is neither a directory nor a symbolic link to a directory, or the path argument contains at least one non-<slash> character and ends with one or more trailing <slash> characters and the last pathname component names an existing file that is neither a directory nor a symbolic link to a directory.";
			break;
		case EPERM:
			errnomsg = "The file named by path is a directory, and either the calling process does not have appropriate privileges, or the implementation prohibits using unlink() on directories.";
			break;
        case EROFS:
            errnomsg = "The directory entry to be unlinked is part of a read-only file system.";
            break;
		case 0:
		default:
			errnomsg = NULL;
			break;
	}
    if(errno) { //se si è verificato un errore
		fprintf(stderr, "\n%s: Error removing file %s\nerrno: [%d] %s\n", __func__, fname, errno, errnomsg); //stampa il messaggio di errore su stderr
	}

    return result;
}

void find(const char *fname) {
    printf("\nFound: %s", fname);
    deletefile(fname);
    if(verify) {
        if(!getyval(" - continue (Yes/No)?")) {
            exit(EXIT_SUCCESS);
        }
    }

    return;
}

int opennew(const char *fname) {    
    if(!access(fname, F_OK)) { //check if the target file already exists
        fprintf(stderr, "\nWarning destination file %s exist\n", fname);
        if(!getyval("Overwrite (Yes/No)?")) { //confirm overwriting
            return -1;
        }        
    }
 
    return open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0600);
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

bool file_info(const char *filename) {
    struct stat sb;
    char *msg;

    if(lstat(filename, &sb) == -1) {
        fprintf(stderr, "\n%s: Stat error in %s\n", __func__, filename);
        
        return false;
    }
    switch(sb.st_mode & S_IFMT) { //point msg to right message file info
        case S_IFBLK:
            msg = "block device";
            break;
        case S_IFCHR:
            msg = "character device";
            break;
        case S_IFDIR:
            msg = "directory";
            break;
        case S_IFIFO:
            msg = "FIFO/pipe";
            break;
        case S_IFLNK:
            msg = "symlink";
            break;
        case S_IFREG:
            msg = "regular file";
            break;
        case S_IFSOCK:
            msg = "socket";
            break;
        default:
            msg = "unknown?";
            break;
    }

    PUTNC(50, '*');
    printf("\n%s\nFile type:                %s\n", filename, msg);
    printf("I-node number:            %ld\n", (long) sb.st_ino);
    printf("Mode:                     %lo (octal)\n", (unsigned long) sb.st_mode);
    printf("Link count:               %ld\n", (long) sb.st_nlink);
    printf("Ownership:                UID=%ld   GID=%ld\n", (long) sb.st_uid, (long) sb.st_gid);
    printf("Preferred I/O block size: %ld bytes\n", (long) sb.st_blksize);
    printf("File size:                %lld bytes\n", (long long) sb.st_size);
    printf("Blocks allocated:         %lld\n", (long long) sb.st_blocks);
    printf("Last status change:       %s\n", ctime(&sb.st_ctime));
    printf("Last file access:         %s\n", ctime(&sb.st_atime));
    printf("Last file modification:   %s\n", ctime(&sb.st_mtime));
    PUTNC(50, '*');
    putchar('\n');

    return true;
}

mode_t filetype(const char *filename) {
    struct stat sb;

    if(lstat(filename, &sb) == -1) {
        return false;
    }

    return (sb.st_mode & S_IFMT);
}

char *buildpath(const char *dirname) {
    char *path, half_path[FILENAME_MAX];

    if(dirname[0] != '.' && dirname[0] != '/') { //if dirname does not begin with . and it doesn't start with / adds a "./" at the beginning of the variable
        sprintf(half_path, "./%s", dirname);
    } else {
        strcpy(half_path, dirname);
    }

    if(half_path[strlen(half_path) -1] != '/') { //verify that the last character is different from slash
        sprintf(path, "%s/", half_path); //adds a slash at the end of the path
    } else {
        path = strdup(half_path); //duplicate the path as it is
    }
    
    free(half_path);

    return path;
}

bool isvalidfilename(const char *filename) {
    mode_t st_mode;
    char dir_template[] = "/tmp/kopyx-tmpdir.XXXXXX", forbidden[] = "/<>\"|:&", *temp_dir, temp_file[FILENAME_MAX];
    FILE *fd;

    if(strpbrk(filename, forbidden)) { //check for invalid characters for filename
        fprintf(stderr, "\n%s is invalid filename!\nPlease avoid to use one or more of the following characters: %s\n", filename, forbidden);

        return false;
    }

    if(strlen(filename) > 255) { //check if the lenght of file exceed the max acceppted by filesystem
        fprintf(stderr, "\n%s is invalid filename, use max %d characters.\n", filename, 255);

        return false;
    }

    if(strpbrk(filename, "*?")) { //check if filname contains wildchars
        wildcard = true;

        return true;
    }

    st_mode = filetype(filename);
    if(st_mode == S_IFREG) { //check if file exist
        return true;
    }

    temp_dir = mkdtemp(dir_template);
    if(temp_dir == NULL) { //creating temp dir
        fprintf(stderr, "\nError creating temporary directory %s", temp_dir);
        exit(EXIT_FAILURE);
    }
    sprintf(temp_file, "%s/%s", temp_dir, filename);

    fd = fopen(temp_file, "w"); //try to create an empty file using "filename"
    if(fd == NULL) {
        if(rm(temp_dir)) {
            fprintf(stderr, "\nError removing temporary directory %s\n", temp_dir);
            exit(EXIT_FAILURE);
        }
        
        return false;
    }
    fclose(fd);

    st_mode = filetype(temp_file);
    if(rm(temp_file)) {
        fprintf(stderr, "\nError removing temporary file %s\n", temp_file);
        exit(EXIT_FAILURE);
    }
    if(rm(temp_dir)) {
        fprintf(stderr, "\nError removing temporary directory %s\n", temp_dir);
        exit(EXIT_FAILURE);
    }

    return (st_mode == S_IFREG);
}