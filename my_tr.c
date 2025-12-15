#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    enum { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode = MODE_SUBST;
    unsigned char prev = 0; //Es declara prev com un caracter buit
    int have_prev = 0; //Es declara have_prev com un nombre enter 0

    if (argc == 1) { //Comprovem si argc és igual a 1
        mode = MODE_SUBST; // Si ho és, entrem al mode de substitució
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        mode = MODE_DELETE; // Si argc és 2 i l'argument és "-d", entrem al mode d'eliminació
    } else if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        mode = MODE_SQUASH; //Si argc és 2 i l'argument és "-s", entrem al mode de compressió
    } else { //Si no es compleix cap de les condicions anteriors, mostrem les formes d'invocarlo
    fprintf(stderr,
        "Ús:\n  %s            substitueix ' ' per ,\n  %s -d         elimina espais\n  %s -s         elimina repeticions consecutives\n",
        argv[0], argv[0], argv[0]); //Utilitzem argv[0] per mostrar el nom del programa
        return 1; //Retornem 1 per indicar que hi ha hagut un error
    }
    unsigned char buf[4096]; //Declarem un buffer de 4096 bytes per a la lectura
    ssize_t n; // Declarem n com un nombre enter de mida signada

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) { //Llegim des del principi fins que no hi hagi més dades
        for (ssize_t i = 0; i < n; ++i) { // Iterem cada byte llegit
            unsigned char c = buf[i]; //Assignem el byte llegit a la variable c

            if (mode == MODE_SUBST) { // Si estem en mode de substitució
                if (c == '\t') c = ','; //Substituïm els espais per comes
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; } //Escrivim el caràcter modificat a la sortida
            } else if (mode == MODE_DELETE) { //Si estem en mode d'eliminació
                if (c == ' ') continue; // I el caràcter és un espai, el saltem
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; } //Escrivim el caràcter a la sortida 
            } else { //Aquest és el MODE_SQUASH
                if (have_prev && c == prev) continue; //Si tenim un caràcter anterior i és igual al caràcter actual, el saltem
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; } //Escrivim el caràcter a la sortida
                prev = c; //Actualitzem el caràcter anterior (es modifica de 0 a c)
                have_prev = 1; //Indiquem que ara tenim un caràcter anterior (es modifica de 0 a 1)
            }
        }
    }
    if (n < 0) { perror("read"); return 1; } //Comprovem si hi ha hagut un error en la lectura
    return 0; //Retornem 0 per indicar que el programa ha finalitzat correctament
}
