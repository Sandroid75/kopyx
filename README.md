
# kopyx
This is Linux version of a old project writed in 1991 for DOS 3.3 (Kopy was the original name), this modern version has some improved features also in therms of speed. Developed with VSCode, you can clone first than open the project folder and press Ctrl+Shift+B to build it.

    KOPYX is a utility that copies and/or deletes from source path
    and/or sub-dir all files that match the specified filter.
    If no destination is specified, the current dir will be used.
    These options can be also specified:
     -d  delete after copying or finding (asks for confirmation)
     -f  just search (don't copy)
     -i  show all info of source file
     -r  also searches subdirectories
     -v  prompts for confirmation before copying each file
     -s  redirects the output to the screen (it can also be redirected with '>')
     -y  doesn't ask for confirmation to delete (use with care!)
    Examples:
    kopyx -dr "*.dat" /              copies all .dat files from / and all sub-dirs
                                     on the current dir ./ and delete the originals.
    kopyx -dy "*.dat" /home/ /bkup   copies all .dat files from /home to /bkup
                                     directory and delete the source files.
    kopyx -fd "*.tmp" /media/search  for all .tmp files in directory /media/
                                     than asks for confirmations before deleting.;
***
## Copyleft 1991-2024 by Sandroid75
***
This code was made by Sandroid75 to illustrate various C language powers.
My recommendations for developing robust C code are:
 - first of all read The C Programming Language: ANSI C Version by K&R (C);
 - after read K&R keep always in mind rules and methods;
 - one of the most characteristics and powers of the C is the pointers;
 - pointers are very helpfull and flexibile to write efficient code;
 - pointers can be dangerous for stable code if you forget the rules;
 - if you use pointers for list of datas don't forget to reserve memory;
 - if you use pointers for list of datas don't forget to release memory;
 - write well-formatted code;
 - only good formatted code makes the code readable;
 - good formatting code reduces the risk of errors;
 - good formatting code facilitates the debugging;
 - good formatting code facilitates the maintenences;
 - good formatting code facilitates future implementantions;
 - commenting the code is another good and necessary practice;
 - commenting the code means understanding what you are doing;
 - commenting the code means understanding what you have done;
 - commenting the code is not a waste of time;
 - at the last but not least, remember rules and methods;

***
_Have fun with C programming by Sandroid75 !!!_
