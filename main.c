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
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <libgen.h>
#include <stdbool.h>
#include <getopt.h>
#include "kopyx.h"

int main(int argc, char *argv[]) {
    char *pattern, *ptrstr, *sourcedir, *sourcefile, *destdir;
    int opt, secarg, last;
    mode_t st_mode;

    opterr = true;
    while((opt = getopt(argc, argv, "dfisvry")) != -1) { //setta tutte le opzioni passate sulla riga di comando
        switch(opt) {
            case 'd': //Cancella il file sorgente (se ha la conferma dell'utente) (default non cancella il file sorgente)
                delete = true;
                break;
            case 'f': //Cerca soltanto  (default copia anche)
                find_only = true;
                break;
            case 'i': //Visualizza tutte le info del file sorgente
                info = true;
                break;
            case 'r': //Estende la ricerca alla sottodirectory specificata (default usa solo la directory specificata)
                include_subdirs = true;
                break;
            case 'v': //Chiede una verifica prima di continuare (default continua - la cancellazione richiede sempre conferma)
                verify = true;
                break;
            case 's': //Redirige la copia sulla periferica di standard output, dalla quale puo' essere rediretto tramite il simbolo di redirezione ">"
                standardoutput = true;
                break;
            case 'y': //non chiede conferma prima di cancellare
                noconfirm = true;
                break;
            case '?': //l'opzione inserita non è valida
                if(isprint(optopt)) {
                    printf("\n%s opzione non valida %c\n", argv[0], (char) optopt);
                } else {
                    printf("\n%s opzione non valida `\\x%x'\n", argv[0], optopt);
                }
                arg_error();
                break;
            default:
                break;
        }
    }

    if(argc <= optind) {//non sono stati passati argomenti
        fprintf(stderr, "\n%s\nSpecificare almeno un file o una directory sorgente\n", argv[0]);
        arg_error();
    } else if((argc - optind) > 2) {//sono stati specificati troppi argomenti
        fprintf(stderr, "\n%s\nTroppi argomenti specificare un file o una directory sorgente ed eventualmente una directory destinazione\n", argv[0]);
        arg_error();
    }

    pattern = strdup(argv[optind]); //duplica l'argomento in pattern    
    st_mode = filetype(pattern); //assegna a st_mode un valore per verificare se l'argometno è una directory o un file (può includere il percorso del file)
    switch(st_mode) {
        case S_IFREG: //se l'argomento continene un percorso completo di un file
            if(strcmp(dirname(pattern), "/")) { //verifica che la dir sia diversa dalla root /
                asprintf(&sourcedir, "%s/", dirname(pattern)); //costruisce il percorso aggiungendo lo slash finale
            } else {
                sourcedir = strdup(dirname(pattern)); //restituisce solo la parte del nomepercorso
            }
            sourcefile = strdup(basename(pattern)); //restituisce solo il nome del file senza percorso
            free(pattern);
            break;
        case S_IFDIR: //se l'argomento contiene una directory SENZA nome di un file
            if(strcmp(pattern, "/")) { //verifica che la dir sia diversa dalla root /
                for(ptrstr = pattern; *ptrstr; ptrstr++); //incrementa il puntatore ptrstr fino a fine stringa
                ptrstr--; //adesso il puntatore ptrstr punterà all'ultimo carattere della stringa pattern
                //il ciclo elimina tutti icaratteri / alla fine della stringa
                while(ptrstr != pattern && *ptrstr == '/') { //se ptrstr non punta all'inizio della stringa pattern, quindi non è la directory / e se l'ultimo carattere dell'argomento è /
                    *ptrstr = '\0'; //allora tronca la stringa eliminanto l'ultimo carattere /
                    ptrstr--; //decrementa il puntatore al carattere precedente
                }
                asprintf(&sourcedir, "%s/", pattern); //assegna il path e aggiunge lo slash
            } else {
                sourcedir = strdup(pattern); //duplica il path
            }
            asprintf(&sourcefile, "*"); //non è stato specificato un file sorgente copierà tutto il contenuto della directory specificand *
            free(pattern);
            break;
        case false: //il file non esiste nella directory specificata
            if(!include_subdirs) { //se non è stato specificato di cercare nelle subdirectory
                fprintf(stderr, "\nFile \'%s\': non esiste nella directory specificata, usa -r per cercare nelle sub-dir\n", pattern);
                free(pattern);
                exit(1);
            }
            sourcedir = strdup(dirname(pattern)); //restituisce solo la parte del nomepercorso
            if(strcmp(sourcedir, "/")) {//verifica che la dir sia diversa dalla root /
                strcat(sourcedir, "/"); //aggiunge lo slash
            }
            sourcefile = strdup(basename(pattern)); //restituisce solo il nome del file senza percorso
            free(pattern);
            break;
        default:
            fprintf(stderr, "\nSorgente: \'%s\' non valida\n", pattern);
            free(pattern);
            exit(EXIT_FAILURE);
            break;
    }

    if(sourcedir[0] != '.' && sourcedir[0] != '/') { //se la directory non comincia con . e non comincia con / aggiunge un "./" allinizio della variabile
        asprintf(&pattern, "./%s", sourcedir);
        free(sourcedir);
        sourcedir = strdup(pattern);
        free(pattern);
    }
    
    if(!(argv[optind+1])) {//se non è specificata la destinazione definisce la current dir
        destdir = strdup("./");
    } else {
        secarg = optind +1; //indice del secondo argomento
        last = strlen(argv[secarg]) -1; //punta all'ultimo carattere prima del NULL
        
        if(argv[secarg][last] != '/') { //verifica che l'ultimo carattere sia diverso da slash
            asprintf(&pattern, "%s/", argv[secarg]); //aggiunge uno slash alla fine del percorso
        } else {
            pattern = strdup(argv[secarg]); //duplica il percorso così com'è
        }
        
        if(pattern[0] != '.' && pattern[0] != '/') { //verifica che il primo carattere non sia ne . ne /
            asprintf(&destdir, "./%s", pattern); //inserisce il ./ come percorso relativo alla posizione corrente
        } else {
            destdir = strdup(pattern); //duplica il percorso così com'è
        }
        free(pattern);
    }
    
    copyall(sourcedir, sourcefile, destdir); //funzione principale di ricerca e copia

    if(!found_one) { //se non è stato trovato il file origine
        fprintf(stderr, "Nessun file trovato\n");
        if(!include_subdirs) { //se non è stato specificato di ricercare nelle subdirectory
            fprintf(stderr, " se usi l'opzione -r puoi cercare nelle sub-directory di %s\n", sourcedir);
            fprintf(stderr, " oppure puoi specificare /%s per cercare nell'intero disco.\n", sourcefile);
        }
    }
    free(sourcedir);
    free(sourcefile);
    free(destdir);
    
    exit(EXIT_SUCCESS);
}
