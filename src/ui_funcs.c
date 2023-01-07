#include "ui_funcs.h"
#include "os_funcs.h"

/*
get single key input, no need to press enter
return kyepressed
*/
static int getch(FILE *streamin)
{
	struct termios ttystate, ttynew;
	int cin;

	tcgetattr(STDIN_FILENO, &ttystate); //get the terminal state
	ttynew = ttystate; //store original terminal mode
	ttynew.c_lflag &= ~ICANON & ~ECHO; //turn OFF canonical mode and echo mode
	tcsetattr(STDIN_FILENO, TCSANOW, &ttynew); //set the terminal attributes

	cin = fgetc(streamin); //get char from streamin

	tcsetattr(STDIN_FILENO, TCSANOW, &ttystate); //restore previuos terminal mode

	return cin;
}

int getyval(const char *msg)
{
	char entered;

	if (noconfirm) { //if the user specified the -y option
		return true;
	}

	fprintf(stderr, "%s", msg);
	entered = getch(stdin);
	if (entered == 0x03) {
		exit(EXIT_FAILURE);
	}

	return (tolower(entered) == 'y'); //return true if user input 'Y' or 'y'
}

bool diskspace(const char *source, const char *dest)
{
	ssize_t totalfilebytes, diskavailbytes;

	totalfilebytes = (ssize_t)totalfilesize(source); //calculate file size
	diskavailbytes = (ssize_t)totaldiskspace(dest); //calculates the total space on the destination disk

	if (diskavailbytes <= totalfilebytes) {
		fprintf(stderr, "\nInsufficient space on: \n%s\n\nplease free some space before trying again.\n", dest);
		fprintf(stderr, "Total space required for the copy: %ld bytes\n", totalfilebytes);
		fprintf(stderr, "Space available on the destination drive: %ld bytes\n", diskavailbytes);
		fprintf(stderr, "Minimum space to free before copying: %ld bytes\n", diskavailbytes - totalfilebytes);

		return false;
	}

	return true;
}

void showtoscreen(const char *from)
{
	size_t bytes;
	char buffer[NSECT * BYTES];
	FILE *fptr;

	if (verify) {
		printf("File: %s", from);
		if (!getyval(" - Show (Yes/No)? ")) {
			return;
		}
		puts("");
	}

	fptr = fopen(from, "r");
	if (!fptr) {
		fprintf(stderr, "\n%s: Error opening file %s\n", __func__, from);
		getyval("Press a key to continue...");
		puts("");

		return;
	}

	while ((bytes = fread(buffer, sizeof(char), NSECT * BYTES, fptr)) > 0) {
		fwrite(buffer, sizeof(char), bytes, stdout);
	}
	fclose(fptr);
	deletefile(from);

	return;
}

void find(const char *fname)
{
	printf("Found: %s\n", fname);
	deletefile(fname);
	if (verify && !getyval(" - copy (Yes/No)?")) //if the user specified -v
		exit(EXIT_SUCCESS);

	return;
}

void deletefile(const char *fname)
{
	if (delfile) {
		printf("File: %s", fname);
		fflush(stdout);
		if (getyval(" - delete (Yes/No)? ")) {
			if (!rm(fname)) {
				printf("\n%s deleted!", fname);
			}
		}
		puts("");
	}

	return;
}

void arg_error(void)
{
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

	getyval("Press a key to continue...");
	puts("");

	exit(EXIT_FAILURE);
}