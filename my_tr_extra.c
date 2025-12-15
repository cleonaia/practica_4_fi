/*
 * Nom del programa: my_tr_extra
 * Descripció: Versió estesa de la comanda 'tr' amb funcionalitats addicionals
 * Autor: Estudiant de Fonaments d'Informàtica
 * Data: Desembre 2024
 * Assignatura: Fonaments d'Informàtica - Pràctica 4
 * 
 * Funcionalitats:
 *   - Sense "-d" ni "-s": rep dos caràcters c1 c2 i substitueix c1 -> c2 a la sortida.
 *     Ex.: echo "miss" | ./my_tr_extra s z    => mizz
 *   - Amb "-d c": elimina el caràcter c.
 *     Ex.: echo "a b c" | ./my_tr_extra -d \s => abc   (\s representa espai)
 *   - Amb "-s": elimina repeticions consecutives del mateix caràcter.
 *     Ex.: echo "baall" | ./my_tr_extra -s     => bal
 *
 * Entrada/Sortida:
 *   - I/O a baix nivell (read/write) sobre stdin (fd=0) i stdout (fd=1)
 * 
 * Compilació: gcc -Wall -Wextra -O2 -std=c11 -o my_tr_extra my_tr_extra.c
 * Ús:
 *   ./my_tr_extra c1 c2     # substitueix c1 -> c2
 *   ./my_tr_extra -d c      # elimina el caràcter c
 *   ./my_tr_extra -s        # elimina repeticions consecutives
 */

#include <unistd.h>   // read, write
#include <string.h>   // strcmp
#include <stdlib.h>   // EXIT_SUCCESS/EXIT_FAILURE
#include <stdio.h>    // perror, fprintf

// Converteix un argument de línia d'ordres en un únic caràcter.
// Accepta: "q" (un sol caràcter), seqüències escapades: \t, \n, \r, \s (espai), \\ (barra invertida), \, (coma)
static int parse_char(const char *s, unsigned char *out) {
    if (!s || !s[0]) return -1;
    if (s[0] == '\\') {
        if (s[1] == 't' && s[2] == '\0') { *out = '\t'; return 0; }
        if (s[1] == 'n' && s[2] == '\0') { *out = '\n'; return 0; }
        if (s[1] == 'r' && s[2] == '\0') { *out = '\r'; return 0; }
        if (s[1] == 's' && s[2] == '\0') { *out = ' ';  return 0; }
        if (s[1] == '\\' && s[2] == '\0') { *out = '\\'; return 0; }
        if (s[1] == ',' && s[2] == '\0') { *out = ',';  return 0; }
        return -1;
    }
    if (s[1] != '\0') return -1; // ha de ser exactament un caràcter
    *out = (unsigned char)s[0];
    return 0;
}

int main(int argc, char *argv[]) {
    enum Mode { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode;
    unsigned char from = 0, to = 0, del = 0;

    // Gestió d'arguments segons l'enunciat de la funcionalitat extra
    if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        mode = MODE_SQUASH;
    } else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        if (parse_char(argv[2], &del) != 0) {
            fprintf(stderr, "Error: caràcter d'eliminació invàlid '%s'\n", argv[2]);
            return EXIT_FAILURE;
        }
        mode = MODE_DELETE;
    } else if (argc == 3) {
        if (parse_char(argv[1], &from) != 0 || parse_char(argv[2], &to) != 0) {
            fprintf(stderr, "Error: caràcters de substitució invàlids '%s' '%s'\n", argv[1], argv[2]);
            return EXIT_FAILURE;
        }
        mode = MODE_SUBST;
    } else {
        fprintf(stderr, "Ús:\n");
        fprintf(stderr, "  %s c1 c2     # substitueix c1 -> c2\n", argv[0]);
        fprintf(stderr, "  %s -d c      # elimina el caràcter c (\\t, \\n, \\r, \\s, \\\\, \\, per especials)\n", argv[0]);
        fprintf(stderr, "  %s -s        # elimina repeticions consecutives\n", argv[0]);
        return EXIT_FAILURE;
    }

    unsigned char buf[1024];
    ssize_t nread;
    unsigned char prev = 0;
    int have_prev = 0;

    while ((nread = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < nread; ++i) {
            unsigned char c = buf[i];

            if (mode == MODE_SUBST) {
                if (c == from) c = to;
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
            } else if (mode == MODE_DELETE) {
                if (c == del) continue; // no escriure aquest caràcter
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
            } else { // MODE_SQUASH
                if (have_prev && c == prev) continue; // eliminar repetició consecutiva
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
                prev = c;
                have_prev = 1;
            }
        }
    }

    if (nread < 0) {
        perror("read");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
