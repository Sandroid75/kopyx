#include "os_funcs.h"

long long totalfilessize(const char *filename) {
    long long filesize;
    FILE *fp = fopen(filename, "r");

    if(fp == NULL) {
        fprintf(stderr, "\n%s: Error opening %s\n", __func__, filename);
        getyval("Press a key to continue...");
        puts("\n");

        exit(EXIT_FAILURE);
    }
    fseek(fp, 0L, SEEK_END);
    filesize = (long long) ftell(fp);
    fclose(fp);

    return (long long) filesize;
}

long long totaldiskspace(const char *todir) {
    struct statvfs diskinfo;
    
    statvfs(todir, &diskinfo); //get disk attributes

    return (long long) diskinfo.f_bsize * diskinfo.f_bavail; //calculates the available disk space
}

mode_t filetype(const char *filename) {
    struct stat sb;

    if(lstat(filename, &sb) == -1) {
        return false;
    }

    return (sb.st_mode & S_IFMT);
}

int opennew(const char *fname) {    
    if(access(fname, F_OK) == 0) { //check if the target file already exists
        fprintf(stderr, "Warning destination file %s exist\n", fname);
        if(getyval("Overwrite (Yes/No)? ")) { //confirm overwriting
            fprintf(stderr, "Overwriting...\n");
            rm(fname);
        } else {
            return -1;
        }      
    }
 
    return open(fname, O_CREAT | O_WRONLY | O_TRUNC, 0600);
}

ssize_t filecopy(const char *source, const char *todir) {
    int input, output;
    struct stat fileinfo = { 0 };
	ssize_t result = -1L;
    off_t bytesCopied = 0;
	char *errnomsg, *filename, destination[FILENAME_MAX];

    filename = strdup(source); // duplicate because of basename() modify the string
    sprintf(destination, "%s%s", todir, basename(filename)); //build complete destination path dir+fname

    if(verify) { //if the user specified -v
        if(!getyval(" - copy (Yes/No)?")) {

            return result;
        }
    }

    if((input = open(source, O_RDONLY)) == -1) { //try opening the source file
        fprintf(stderr, "\n%s: File open error: %s\n", __func__, source);
        getyval("Press a key to continue...");
        puts("\n");

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
        result = (getyval("Continue (Yes/No)?") ? result : -1L); //if the user confirms, the copying of other files continues
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
    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));
    PUTNC(50, '*');
    putchar('\n');

    return true;
}

char *buildpath(const char *dirname) {
    char *path, half_path[FILENAME_MAX];

    if(dirname[0] != '.' && dirname[0] != '/') { //if dirname does not begin with . and it doesn't start with / adds a "./" at the beginning of the variable
        sprintf(half_path, "./%s", dirname);
    } else {
        strcpy(half_path, dirname);
    }

    if(half_path[strlen(half_path) -1] != '/') { //verify that the last character is different from slash
        asprintf(&path, "%s/", half_path); //adds a slash at the end of the path
    } else {
        path = strdup(half_path); //duplicate the path as it is
    }

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

ino_t inodeof(const char *filename) {
    struct stat sb;

    if(lstat(todir, &sb) == -1) {
            fprintf(stderr, "\n%s : error reading i-node number!\n", todir);
            free(todir);

            exit(EXIT_FAILURE);
    }

    return (ino_t) sb.st_ino;
}