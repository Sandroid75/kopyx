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
#include "kopyx.h"

void copyall(const char *fromdir, const char *filename, const char *todir) {
    char *spath, *tpath;
    intptr_t handle;
    ssize_t totalfilesbytes, diskavailbytes;
    struct _finddata_t block;
    
    diskavailbytes = totaldiskspace(todir); //calculates the total space on the destination disk
    totalfilesbytes = totalfilessize(fromdir, filename);

    if(diskavailbytes <= totalfilesbytes) {
        fprintf(stderr, "Insufficient space on: \n%s\n\nplease free some space before trying again.", todir);
        fprintf(stderr, "Total space required for the copy: %ld bytes\n", totalfilesbytes);
        fprintf(stderr, "Space available on the destination drive: %ld bytes\n", diskavailbytes);
        fprintf(stderr, "Minimum space to free before copying: %ld bytes\n", diskavailbytes - totalfilesbytes);

        return;        
    }

    asprintf(&spath, "%s%s", fromdir, filename); //builds the full path + filename
    handle = _findfirst(spath, &block); //try to read the info of the file
    free(spath);
    
    if(handle != -1L) { //if it found a valid file
        do {
            found_one = true; //set the global variable to specify that a valid source file was found

            asprintf(&spath, "%s%s", fromdir, block.name); //builds full path + source filename
            asprintf(&tpath, "%s%s", todir, block.name); //builds the full path + destination filename

            if(info) { //if it specified to show file info
                file_info(spath);
            }

            if(find_only) { //if it have specified to search only
                find(spath);
            } else if(standardoutput) { //redirect the output to the screen
                showtoscreen(spath);
            } else if(strcmp(spath, tpath)) { //otherwise if origin and destination do not coincide, copy
                filecopy(spath, tpath); //copy the file to the destination
            } else {//if origin and destination coincide
                found_one = false; //resets the global variable again
            }
            
            free(spath);
            free(tpath);
        } while(!_findnext(handle, &block));
    }
    _findclose(handle);
    
    if(include_subdirs) { //if it has specified to also search in subdirectories
        asprintf(&spath, "%s/*", fromdir); //builds full path + source filename
        handle = _findfirst(spath, &block); //check the file info
        free(spath);
        
        if(handle != -1L) {
            do {
                if(block.attrib & _A_SUBDIR) //if a subdirectory was found
                    if(block.name [0] != '.' && block.name[0] != '/') { //and the first character is not a special character
                        asprintf(&spath, "%s/%s/", fromdir, block.name); //builds the complete path by adding the one found
                        copyall(spath, filename, todir); //recursion to search subdirectories
                        free(spath);
                    }
            } while(!_findnext(handle, &block));
        }
        _findclose(handle);
     }

     return;
}

long long totaldiskspace(const char *todir) {
    struct statvfs diskinfo;
    
    statvfs(todir, &diskinfo); //get disk attributes
    return (long long) diskinfo.f_bsize * diskinfo.f_bavail; //calculates the available disk space
}

long long totalfilessize(const char *fromdir, const char *filename) {
    char *spath;
    intptr_t handle;
    struct _finddata_t block;
    struct stat sb;
    unsigned long totalspace = 0L;

    asprintf(&spath, "%s%s", fromdir, filename); //builds the full path + filename
    handle = _findfirst(spath, &block); //try to read the info of the file
    free(spath);
    
    if(handle != -1L) { //if it found a valid file
        do {
            asprintf(&spath, "%s%s", fromdir, block.name); //builds full path + source filename

            if(lstat(spath, &sb) == -1) { //query the file stats
                fprintf(stderr, "\nError stat on %s\n", filename);

                return -1L;
            }
            if((sb.st_mode & S_IFMT) == S_IFREG) { //verify that it is a regular file
                totalspace += sb.st_size; //increment the bytes counter
            }

            free(spath);
        } while(!_findnext(handle, &block));
    }
    _findclose(handle);

    return (long long) totalspace;
}

void showtoscreen(const char *from) {
    size_t bytes;
    char buffer[NSECT * BYTES];
    FILE *fptr;
    
    if(verify) {
        fprintf(stderr, "File: %s", from);
        if(!getyval(" - Show (Yes/No)?")) {
            return;
        }
        fprintf(stderr, "\n");
    }

    fptr = fopen(from, "r");
    if(!fptr) {
        fprintf(stderr, "Error opening file %s\n", from);
        
        return;
    }

    while((bytes = fread(buffer, sizeof(char), NSECT * BYTES, fptr)) > 0) {
        fwrite(buffer, sizeof(char), bytes, stdout);
    }

    fclose(fptr);
    
    if(delete) {
        fprintf(stderr, "File: %s", from);
        if(getyval(" - delete (Yes/No)?")) {
            if(!rm(from)) { //delete the source file
                fprintf(stderr, "deleted!\n");
            }
        }
    }

    return;
}

ssize_t filecopy(const char *source, const char *destination) {
    int input, output;
    struct stat fileinfo = { 0 };
	ssize_t result = -1L;
    off_t bytesCopied = 0;
	char *errnomsg;
	//_Bool check = false, faliure = false;

    fprintf(stderr, "\nCoping %s -> %s", source, destination);

    if(verify) { //if the user specified -v
        if(!getyval(" - copy (Yes/No)?")) {

            return result;
        }
    }

    if((input = open(source, O_RDONLY)) == -1) { //try opening the source file
        fprintf(stderr, "%s: Read error: %s\n", __func__, source);

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
        fprintf(stderr, "File verification error %s\nerrno: [%d] %s\n", source, errno, errnomsg);

        return result;
    }

    if((output = opennew(destination)) == -1) { //try to open the target file
        close(input);
        fprintf(stderr, "Write error: %s\n", destination);

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
		fprintf(stderr, "File copy error %s\nerrno: [%d] %s\n", source, errno, errnomsg); //print the error message on stderr
	}
	if(result != fileinfo.st_size) { //verify that all bytes have been copied
		fprintf(stderr, "Error: not all data was copied!\nWrited %ld bytes on %ld bytes\n", result, fileinfo.st_size);
        result = getyval("Continue (Yes/No)?") ? result : -1L; //if the user confirms, the copying of other files continues
	}

    close(input); //close the handle
    close(output); //close the handle

    if(delete) {
        fprintf(stderr, "File: %s", source);
        if(getyval(" - delete (Yes/No)?")) {
            if(!rm(source)) { //delete the source file
                fprintf(stderr, "deleted!\n");
            }
        }
    }

    return result; //return the number of bytes copied
}

int rm(const char *fname) {
    int result;
    char *errnomsg;

    errno = 0;
    result = remove(fname);

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
		fprintf(stderr, "Error removing file %s\nerrno: [%d] %s\n", fname, errno, errnomsg); //stampa il messaggio di errore su stderr
	}

    return result;
}

void find(const char *fname) {
    fprintf(stderr, "Found: %s", fname);
    if(delete) {
        fprintf(stderr, "File: %s", fname);
        if(getyval(" - delete (Yes/No)?")) {
            if(!rm(fname)) { //delete the source file
                fprintf(stderr, "\ndeleted!\n");
            }
        }
    }
    if(verify) {
        if(!getyval(" - continue (Yes/No)?")) {
            exit(EXIT_SUCCESS);
        }
    }

    return;
}

void closefiles(FILE *fptr, FILE *tptr) {
    fclose(fptr);
    if(!standardoutput) {
        fclose(tptr);
    }
}

int opennew(const char *fname) {    
    //int fhandle;

    if(!access(fname, F_OK)) { //check if the target file already exists
        fprintf(stderr, "Error: destination file %s exist\n", fname);
        if(!getyval("Overwrite (Yes/No)?")) { //confirm overwriting
            return -1;
        }        
    }
 
    return open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0600);
}

void arg_error(void) {
    fprintf(stderr, "%s", errmsg);    

    getyval("\nPress a key to continue...");
    
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

int file_info(const char *file_name) {
    struct stat sb;
    int i;

    if(lstat(file_name, &sb) == -1) {
        fprintf(stderr, "\nStat error in %s\n", file_name);
        
        return false;
    }
    
    for(i = 0; i < 50; i++) {
        putchar('*');
    }    
    printf("\n%s\nFile type:                ", file_name);    
    switch(sb.st_mode & S_IFMT) {
        case S_IFBLK:
            printf("block device\n");
            break;
        case S_IFCHR:
            printf("character device\n");
            break;
        case S_IFDIR:
            printf("directory\n");
            break;
        case S_IFIFO:
            printf("FIFO/pipe\n");
            break;
        case S_IFLNK:
            printf("symlink\n");
            break;
        case S_IFREG:
            printf("regular file\n");
            break;
        case S_IFSOCK:
            printf("socket\n");
            break;
        default:
            printf("unknown?\n");
            break;
    }

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

    for(i = 0; i < 50; i++) {
        putchar('*');
    }
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
