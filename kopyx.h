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

#ifndef KOPY_H_
#define KOPY_H_

#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include "findfirst.h"

static char errmsg[] = 
  "\nKOPYX è un'utility che copia/cancella dal path sorgente e/o dalle sub-dir\n"
  "tutti i file che corrispondono al filtro specificato es: kopyx -dfr *.tmp\n"
  "Se non viene specificata la destinizaione, verrà utilizzata la dir corrente.\n"
  "Possono essere specificate queste opzioni:\n\n"
  " -d  cancella dopo avere copiato/trovato (chiede conferma)\n"
  " -f  cerca soltanto (non copia)\n"
  " -i  visualizza tutte le info del file sorgente\n"
  " -r  cerca anche nelle sub-directory\n"
  " -v  richiede la conferma prima di copiare ogni file\n"
  " -s  redirige sullo schermo l'output (puo' essere a sua volta rediretto con >)\n"
  " -y  non chiede conferma per cancellare (usare con attenzione!)\n\n"
  "Esempi:\n\n"
  "kopyx -dr /*.dat             copia tuttti i file .dat da / e tutte le sub-dir\n"
  "                             del disco e cancella gli originali.\n\n"
  "kopyx -d /home/*.dat /bkup   copia tutti i file .dat da /home\n"
  "                             alla directory /bkup e cancella gli originali.\n\n"
  "kopyx -fd /media/*.bak       cerca tutti i file *.bak nella directory /media/\n"
  "                             chiede conferma prima di cancellare.\n\n"
  ;

void copyall(const char *fromdir, const char *filename, const char *todir);
long long totalfilessize(const char *fromdir, const char *filename);
long long totaldiskspace(const char *todir);
void showtoscreen(const char *from);
ssize_t filecopy(const char *source, const char *destination);
void find(const char *fname);
void closefiles(FILE *fptr, FILE *tptr);
int opennew(const char *fname);
int rm(const char *fname);
void arg_error(void);
int getyval(const char *msg);
int file_info(const char *file_name);
mode_t filetype(const char *filename);
//int stricmp(char const *, char const *);

static _Bool found_one = false, delete = false, find_only = false, verify = false, standardoutput = false, info = false, include_subdirs = false, noconfirm = false;

#define NSECT 64
#define BYTES 512

#endif // KOPY_H
