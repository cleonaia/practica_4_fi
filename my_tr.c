/*
 * Nom del programa: my_tr
 * Descripció: Programa simplificat similar a la comanda 'tr' d'Unix
 * Autor: Estudiant de Fonaments d'Informàtica
 * Data: Desembre 2024
 * Assignatura: Fonaments d'Informàtica - Pràctica 4
 * 
 * Funcionalitat:
 *   - Sense arguments: substitueix espais ' ' per comes ','
 *   - Amb -d: elimina tots els espais ' ' de l'entrada
 *   - Amb -s: elimina caràcters consecutius duplicats
 * 
 * Entrada/Sortida:
 *   - Llegeix de stdin utilitzant read() (baix nivell)
 *   - Escriu a stdout utilitzant write() (baix nivell)
 * 
 * Compilació: gcc -Wall -o my_tr my_tr.c
 * Ús:
 *   ./my_tr              # substitueix ' ' per ','
 *   ./my_tr -d           # elimina espais
 *   ./my_tr -s           # elimina repeticions consecutives
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    /* Modes d'operació del programa */
    enum { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode = MODE_SUBST;
    
    /* Variables per al mode SQUASH (eliminació de duplicats consecutius) */
    unsigned char prev = 0;      // Caràcter anterior processat
    int have_prev = 0;            // Flag: hem processat algun caràcter?

    /* Determinació del mode d'operació segons els arguments de línia d'ordres */
    if (argc == 1) {
        /* Sense arguments: mode substitució (espais -> comes) */
        mode = MODE_SUBST;
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        /* Opció -d: mode eliminació (eliminar espais) */
        mode = MODE_DELETE;
    } else if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        /* Opció -s: mode squash (eliminar duplicats consecutius) */
        mode = MODE_SQUASH;
    } else {
        /* Arguments invàlids: mostrar missatge d'ús i sortir */
        fprintf(stderr,
                "Ús:\n  %s            # substitueix ' ' per ','  %s -d         # elimina espais\n  %s -s         # elimina repeticions consecutives\n",
                argv[0], argv[0], argv[0]);
        return 1;
    }

    /* Buffer per llegir dades de stdin */
    unsigned char buf[4096];
    ssize_t n;

    /* Bucle principal: llegir de stdin, processar i escriure a stdout */
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        /* Processar cada byte llegit segons el mode seleccionat */
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char c = buf[i];

            if (mode == MODE_SUBST) {
                /* Mode SUBST: substituir espais per comes */
                if (c == ' ') c = ',';
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else if (mode == MODE_DELETE) {
                /* Mode DELETE: ometre espais, escriure la resta */
                if (c == ' ') continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else { // MODE_SQUASH
                /* Mode SQUASH: ometre caràcters consecutius duplicats */
                if (have_prev && c == prev) continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
                prev = c;
                have_prev = 1;
            }
        }
    }
    
    /* Verificar si hi ha hagut error en la lectura */
    if (n < 0) { perror("read"); return 1; }
    
    return 0;
}
