#include "os_funcs.h"

off_t totalfilesize(const char *filename)
{
    struct stat sb;

    if (lstat(filename, &sb) == -1)
        return false;

    return sb.st_size;
}

size_t totaldiskspace(const char *todir)
{
    struct statvfs diskinfo;

    statvfs(todir, &diskinfo); // get disk attributes

    return diskinfo.f_bsize * diskinfo.f_bavail; // calculates the available disk space
}

mode_t filetype(const char *filename)
{
    struct stat sb;

    if (lstat(filename, &sb) == -1)
        return false;

    return sb.st_mode & S_IFMT;
}

/*
check if fname exist and coincide with suorce_inode than open a new file
return the handle or -1 in case of error, -2 if source == dest, -3 user dont want overwriting
*/
static int opennew(const char *fname, const struct stat sbi)
{
    struct stat sbo;

    if (lstat(fname, &sbo) == 0) { // check if the target file already exists
        if (sbi.st_ino == sbo.st_ino)
            return FC_SAMEF;

        fprintf(stderr, "Warning destination file %s exist\n", fname);
        if (getyval("Overwrite (Yes/No)? ")) { // confirm overwriting
            if (!noconfirm)
                puts("");
            fprintf(stdout, "Overwriting...\n");
            if (rm(fname))
                return FC_NOOWD;
        } else
            return FC_USKIP;
    }

    return open(fname, O_CREAT | O_WRONLY | O_TRUNC, sbi.st_mode);
}

ssize_t filecopy(const char *source, const char *todir)
{
    int input, output;
    struct stat sb = {0};
    ssize_t chunk, bytesCopied, bytesTocopy = 0;
    char *errmsg, *data, *ptr, *end, buffer[FILENAME_MAX], destination[FILENAME_MAX];
    bool tryAgain = false; // Applications may wish to fall back to read(2)/write(2) in the case where sendfile() fails with EINVAL or ENOSYS.

    if (verify && !getyval(" - copy (Yes/No)?")) // if the user specified -v
        return 0L;

    strcpy(destination, todir);
    ADDSLASH(destination);                 // to be sure that the last char is /
    strcat(destination, FILENAME(source)); // build complete destination path dir+fname

    errno = 0;
    if (lstat(source, &sb)) { // read the attributes of the source file
        switch (errno) {
        case EACCES:
            errmsg = "Search permission is denied for one of the directories in the path prefix of source file.";
            break;
        case EBADF:
            errmsg = "fd is not a valid open file descriptor.";
            break;
        case EFAULT:
            errmsg = "Bad address.";
            break;
        case ELOOP:
            errmsg = "Too many symbolic links encountered while traversing the path.";
            break;
        case ENAMETOOLONG:
            errmsg = "Source pathname is too long.";
            break;
        case ENOENT:
            errmsg = "A component of source pathname does not exist or is a dangling symbolic link.";
            break;
        case ENOMEM:
            errmsg = "Out of memory (i.e., kernel memory).";
            break;
        case ENOTDIR:
            errmsg = "A component of the path prefix of pathname is not a directory.";
            break;
        case EOVERFLOW:
            errmsg = "Source pathname refers to a file whose size, inode number, or number of blocks cannot be represented in, respectively, the types off_t, ino_t, or blkcnt_t.\nThis error can occur when, for example, an application compiled on a 32-bit platform without -D_FILE_OFFSET_BITS=64 calls stat() on a file whose size exceeds (1<<31)-1 bytes.";
            break;
        case 0:
        default:
            errmsg = NULL;
            break;
        }
        fprintf(stderr, "\n%s: File verification error %s\nerrno: [%d] %s\n", __func__, source, errno, errmsg);

        return FC_ERROR;
    }

    if (S_ISLNK(sb.st_mode)) {
        chunk = readlink(source, buffer, FILENAME_MAX);
        if (chunk < 0L) {
            fprintf(stderr, "Error reading symbolic link %s\n", source);
            getyval("");
        } else {
            buffer[chunk] = '\0';
            chunk         = symlink(buffer, destination);
        }
        if (chunk < 0) {
            fprintf(stderr, "Error writing symbolic link %s\n", destination);
            getyval("");

            return chunk;
        }

        return sb.st_size;
    }

    if ((input = open(source, O_RDONLY)) < 0) { // try opening the source file
        fprintf(stderr, "\n%s: File open error: %s\n", __func__, source);

        return input;
    }
    if ((output = opennew(destination, sb)) < 0) { // try to open the destinatiion file
        close(input);

        return (ssize_t) output;
    }

    errno       = 0;
    bytesTocopy = sb.st_size;
    bytesCopied = sendfile(output, input, NULL, bytesTocopy); // sendfile will work with non-socket output (i.e. regular file) on Linux 2.6.33+
    switch (errno) {
    case EAGAIN:
        errmsg = "Nonblocking I/O has been selected using O_NONBLOCK and the write would block.";
        break;
    case EBADF:
        errmsg = "The input file was not opened for reading or the output file was not opened for writing.";
        break;
    case EFAULT:
        errmsg = "Bad address.";
        break;
    case EINVAL:
    case ENOSYS:
        errmsg   = "Descriptor is not valid or locked, or an mmap(2)-like operation is not available for in_fd, or count is negative.";
        tryAgain = true; // read NOTES at https://man7.org/linux/man-pages/man2/sendfile.2.html
        break;
    case EIO:
        errmsg = "Unspecified error while reading from in_fd.";
        break;
    case ENOMEM:
        errmsg = "Insufficient memory to read from in_fd.";
        break;
    case EOVERFLOW:
        errmsg = "count is too large, the operation would result in exceeding the maximum size of either the input file or the output file.";
        break;
    case ESPIPE:
        errmsg = "offset is not NULL but the input file is not seekable.";
        break;
    case 0:
    default:
        errmsg = NULL;
        break;
    }
    if (errno)                                                                                          // if an error has occurred
        fprintf(stderr, "\n%s: File copy error %s\nerrno: [%d] %s\n", __func__, source, errno, errmsg); // print the error message on stderr

    if (tryAgain) { // read NOTES at https://man7.org/linux/man-pages/man2/sendfile.2.html
        close(output);
        rm(destination);
        if ((output = opennew(destination, sb)) < 0) { // try to open the destinatiion file
            close(input);

            return (ssize_t) output;
        }
        fprintf(stderr, "The application will try to copy again with a less performing strategy.\n");
        errno = 0;
        chunk = MIN(bytesTocopy, DEFAULT_CHUNK); // set better performance for copy
        data  = malloc((size_t) chunk);          // Allocate temporary data buffer.
        if (data) {
            bytesCopied = 0L;
            do { // read/write loop
                bytesTocopy = read(input, data, chunk);
                if (bytesTocopy <= 0)
                    break; // exit from do while loop

                ptr = data;
                end = (char *) (data + bytesTocopy);
                while (ptr < end) { // write data loop
                    bytesTocopy = write(output, ptr, (size_t) (end - ptr));
                    if (bytesTocopy <= 0) {
                        tryAgain = false; // exit from do while loop
                        break;            // exit from while loop
                    }
                    bytesCopied += bytesTocopy;
                    ptr += bytesTocopy;
                }
            } while (tryAgain);
            FREE(data);

            if (!errno)
                fprintf(stderr, "The new strategy worked, the file was copied!\n");
        }
    }
    close(input);  // close the handle
    close(output); // close the handle

    if (bytesCopied != bytesTocopy) { // verify that all bytes have been copied
        fprintf(stderr, "\n%s: Error not all data was copied!\nWrited %ld bytes on %ld bytes\n", __func__, bytesCopied, bytesTocopy);
        if (getyval("Overwrite (Yes/No)? ")) // confirm overwriting
            rm(destination);
        puts("");
    }
    deletefile(source); // if user specify to move the file

    return bytesCopied; // return the number of bytes copied
}

int rm(const char *fname)
{
    int result = -1;
    mode_t st_mode;
    char *msgerr;

    errno = 0;

    st_mode = filetype(fname);
    if (S_ISREG(st_mode) || S_ISLNK(st_mode))
        result = unlink(fname);
    else if (S_ISDIR(st_mode))
        result = rmdir(fname);
    else
        fprintf(stderr, "\"%s\" is not a regular file, symbolic link or directory.\n", fname);

    switch (errno) {
    case EACCES:
        msgerr = "Write access to the file or directory is not allowed for the process's effective UID, or one of the directories in pathname did not allow search permission.";
        break;
    case EBUSY:
        msgerr = "The file or directory cannot be unlinked because it is being used by the system or another process that prevents its removal.";
        break;
    case EFAULT:
        msgerr = "The file or directory points outside your accessible address space.";
        break;
    case EIO:
        msgerr = "An I/O error occurred.";
        break;
    case ELOOP:
        msgerr = "Too many symbolic links were encountered in translating.";
        break;
    case ENAMETOOLONG:
        msgerr = "pathname was too long.";
        break;
    case ENOENT:
        msgerr = "A component of path does not exist or is a dangling symbolic link, or pathname is empty.";
        break;
    case ENOMEM:
        msgerr = "Insufficient kernel memory was available.";
        break;
    case ENOTDIR:
        msgerr = "A component used as a directory in pathname is not, in fact, a directory.";
        break;
    case EPERM:
        msgerr = "The system does not allow unlinking of directories, or unlinking of directories requires privileges that the calling process doesn't have.";
        break;
    case EROFS:
        msgerr = "The file or directory refers to a file on a read-only filesystem.";
        break;
    case EBADF:
        msgerr = "pathname is relative but dirfd is neither AT_FDCWD nor a valid file descriptor.";
        break;
    case EINVAL:
        msgerr = "An invalid flag value was specified in flags.";
        break;
    case EISDIR:
        msgerr = "pathname refers to a directory, and AT_REMOVEDIR was not specified in flags.";
        break;
    case ENOTEMPTY:
        msgerr = "Dirctory not empty.";
        break;
    default:
        msgerr = "Generic error not specified by OS.";
        break;
    }
    if (errno)                                                                           // error is occurred
        fprintf(stderr, "Error removing \"%s\"\nerrno[%d]: %s\n", fname, errno, msgerr); // write detailed error message to log file

    return result;
}

bool file_info(const char *filename)
{
    struct stat sb;
    char *msg;

    if (lstat(filename, &sb) == -1) {
        fprintf(stderr, "\n%s: Stat error in %s\n", __func__, filename);

        return false;
    }
    switch ((sb.st_mode & S_IFMT)) { // point msg to right message file info
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

char *buildpath(const char *dirname)
{
    char *path, half_path[FILENAME_MAX];

    if (dirname[0] != '.' && dirname[0] != '/') { // if dirname does not begin with . and it doesn't start with / adds a "./" at the beginning of the variable
        sprintf(half_path, "./%s", dirname);
    } else {
        strcpy(half_path, dirname);
    }

    if (half_path[strlen(half_path) - 1] != '/') { // verify that the last character is different from slash
        asprintf(&path, "%s/", half_path);         // adds a slash at the end of the path
    } else {
        path = strdup(half_path); // duplicate the path as it is
    }

    return path;
}

bool isvalidfilename(const char *filename)
{
    mode_t st_mode;
    char dir_template[] = "/tmp/kopyx-tmpdir.XXXXXX", forbidden[] = "/<>\"|:&", extmatch[] = "?*+@!", temp_file[FILENAME_MAX], *temp_dir = NULL;
    FILE *fd;

    if (strpbrk(filename, forbidden)) { // check for invalid characters for filename
        fprintf(stderr, "\n%s is invalid filename!\nPlease avoid to use one or more of the following characters: %s\n", filename, forbidden);

        return false;
    }
    if (strlen(filename) > 255) { // check if the lenght of file exceed the max acceppted by filesystem
        fprintf(stderr, "\n%s is invalid filename, use max %d characters.\n", filename, 255);

        return false;
    }
    if (strpbrk(filename, extmatch)) { // check if filname contains wildchars FNM_EXTMATCH
        wildcard = true;

        return true;
    }
    st_mode = filetype(filename);
    if (st_mode == S_IFREG || st_mode == S_IFLNK) { // check if file exist
        return true;
    }

    temp_dir = mkdtemp(dir_template);
    if (temp_dir == NULL && !mkdir(temp_dir, S_IRWXU | S_IRWXG | S_IRWXO)) { // creating temp dir 0777
        fprintf(stderr, "\nError creating temporary directory %s", dir_template);
        exit(EXIT_FAILURE);
    }

    sprintf(temp_file, "%s/%s", temp_dir, filename);
    fd = fopen(temp_file, "w"); // try to create an empty file using "filename"
    if (fd == NULL) {
        if (rm(temp_dir)) {
            fprintf(stderr, "\nError removing temporary directory %s\n", temp_dir);
            exit(EXIT_FAILURE);
        }

        return false;
    }
    fclose(fd);

    st_mode = filetype(temp_file); // check if the tempo file is regular

    if (rm(temp_file)) {
        fprintf(stderr, "\nError removing temporary file %s\n", temp_file);
        exit(EXIT_FAILURE);
    }
    if (rm(temp_dir)) {
        fprintf(stderr, "\nError removing temporary directory %s\n", temp_dir);
        exit(EXIT_FAILURE);
    }

    return (st_mode == S_IFREG);
}
