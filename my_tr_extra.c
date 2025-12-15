/*
 * Programa: my_tr_extra (Funcionalitat extra)
 * Descripció: Versió avançada del programa my_tr amb suport per substituir
 *             qualsevol caràcter i eliminar caràcters específics.
 * 
 * Funcionalitats: 
 *   - Sense "-d" ni "-s": rep dos caràcters c1 c2 i substitueix c1 -> c2
 *     Ex.:  echo "miss" | ./my_tr_extra s z    => mizz
 *   - Amb "-d c":  elimina el caràcter c de l'entrada
 *     Ex.: echo "a b c" | ./my_tr_extra -d \s => abc   (\s representa espai)
 *   - Amb "-s":  elimina repeticions consecutives del mateix caràcter
 *     Ex.: echo "baall" | ./my_tr_extra -s     => bal
 *
 * Suport per caràcters especials:  \t, \n, \r, \s (espai), \\ (barra), \, (coma)
 * 
 * I/O a baix nivell:  Utilitza read() i write() sobre stdin (fd=0) i stdout (fd=1)
 * 
 * Compilació: gcc -Wall -Wextra -O2 -std=c11 -o my_tr_extra my_tr_extra.c
 * 
 * Exemples d'ús:
 *   ./my_tr_extra a e < text. txt        # Substitueix 'a' per 'e'
 *   ./my_tr_extra -d o < text.txt       # Elimina totes les 'o'
 *   ./my_tr_extra -d \s < text.txt      # Elimina tots els espais
 *   ./my_tr_extra -s < text.txt         # Elimina repeticions consecutives
 */

#include <unistd.h>   // per read(), write(), STDIN_FILENO, STDOUT_FILENO
#include <string.h>   // per strcmp()
#include <stdlib.h>   // per EXIT_SUCCESS, EXIT_FAILURE
#include <stdio.h>    // per fprintf(), perror()

/*
 * Funció:  parse_char
 * Descripció:  Converteix un argument de línia d'ordres en un únic caràcter. 
 *             Accepta caràcters literals i seqüències escapades.
 * 
 * Paràmetres:
 *   - s: cadena d'entrada (argument de línia d'ordres)
 *   - out: punter on es guardarà el caràcter interpretat
 * 
 * Retorna:
 *   - 0 si s'ha interpretat correctament
 *   - -1 si hi ha hagut un error (cadena invàlida)
 * 
 * Seqüències acceptades:
 *   \t  -> tabulador
 *   \n  -> nova línia
 *   \r  -> retorn de carro
 *   \s  -> espai
 *   \\  -> barra invertida
 *   \,  -> coma
 */
static int parse_char(const char *s, unsigned char *out) {
    if (!s || !s[0]) return -1;  // Cadena buida o NULL
    
    // Gestió de seqüències escapades
    if (s[0] == '\\') {
        if (s[1] == 't' && s[2] == '\0') { *out = '\t'; return 0; }
        if (s[1] == 'n' && s[2] == '\0') { *out = '\n'; return 0; }
        if (s[1] == 'r' && s[2] == '\0') { *out = '\r'; return 0; }
        if (s[1] == 's' && s[2] == '\0') { *out = ' ';  return 0; }
        if (s[1] == '\\' && s[2] == '\0') { *out = '\\'; return 0; }
        if (s[1] == ',' && s[2] == '\0') { *out = ',';  return 0; }
        return -1;  // Seqüència escapada invàlida
    }
    
    // Ha de ser exactament un caràcter
    if (s[1] != '\0') return -1;
    
    *out = (unsigned char)s[0];
    return 0;
}

int main(int argc, char *argv[]) {
    // Definició dels modes d'operació
    enum Mode { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode;
    
    // Variables per emmagatzemar els caràcters d'operació
    unsigned char from = 0;  // Caràcter origen (per substituir)
    unsigned char to = 0;    // Caràcter destí (substitució)
    unsigned char del = 0;   // Caràcter a eliminar

    // Gestió d'arguments segons l'enunciat de la funcionalitat extra
    if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        // Mode:  eliminar repeticions consecutives
        mode = MODE_SQUASH;
        
    } else if (argc == 3 && strcmp(argv[1], "-d") == 0) {
        // Mode: eliminar un caràcter específic
        if (parse_char(argv[2], &del) != 0) {
            fprintf(stderr, "Error: caràcter d'eliminació invàlid '%s'\n", argv[2]);
            return EXIT_FAILURE;
        }
        mode = MODE_DELETE;
        
    } else if (argc == 3) {
        // Mode: substituir caràcter origen per caràcter destí
        if (parse_char(argv[1], &from) != 0 || parse_char(argv[2], &to) != 0) {
            fprintf(stderr, "Error: caràcters de substitució invàlids '%s' '%s'\n", argv[1], argv[2]);
            return EXIT_FAILURE;
        }
        mode = MODE_SUBST;
        
    } else {
        // Arguments invàlids: mostrar missatge d'ajuda
        fprintf(stderr, "Ús:\n");
        fprintf(stderr, "  %s c1 c2     # substitueix c1 -> c2\n", argv[0]);
        fprintf(stderr, "  %s -d c      # elimina el caràcter c (\\t, \\n, \\r, \\s, \\\\, \\, per especials)\n", argv[0]);
        fprintf(stderr, "  %s -s        # elimina repeticions consecutives\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Buffer per lectura eficient en blocs
    unsigned char buf[1024];
    ssize_t nread;  // Nombre de bytes llegits
    
    // Variables per controlar repeticions (MODE_SQUASH)
    unsigned char prev = 0;
    int have_prev = 0;

    // Bucle principal: llegir de stdin fins arribar a EOF
    while ((nread = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        // Processar cada byte del buffer
        for (ssize_t i = 0; i < nread; ++i) {
            unsigned char c = buf[i];

            if (mode == MODE_SUBST) {
                // MODE_SUBST: Substituir caràcter 'from' per 'to'
                if (c == from) {
                    c = to;
                }
                // Escriure el caràcter (modificat o original) a stdout
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
                
            } else if (mode == MODE_DELETE) {
                // MODE_DELETE: No escriure si el caràcter coincideix amb 'del'
                if (c == del) {
                    continue;  // Saltar aquest caràcter
                }
                // Escriure els altres caràcters
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
                
            } else {
                // MODE_SQUASH:  Eliminar repeticions consecutives
                if (have_prev && c == prev) {
                    continue;  // Saltar si és igual al caràcter anterior
                }
                // Escriure el caràcter
                if (write(STDOUT_FILENO, &c, 1) != 1) {
                    perror("write");
                    return EXIT_FAILURE;
                }
                // Actualitzar el caràcter anterior
                prev = c;
                have_prev = 1;
            }
        }
    }

    // Comprovar errors de lectura
    if (nread < 0) {
        perror("read");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
