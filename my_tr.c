/*
 * Nombre del programa: my_tr
 * Descripción: Programa simplificado similar al comando 'tr' de Unix
 * Autor: Estudiante de Fonaments d'Informàtica
 * Fecha: Diciembre 2024
 * Asignatura: Fonaments d'Informàtica - Pràctica 4
 * 
 * Funcionalitat:
 *   - Sin argumentos: substituye espacios ' ' por comas ','
 *   - Con -d: elimina todos los espacios ' ' de la entrada
 *   - Con -s: elimina caracteres consecutivos duplicados
 * 
 * Entrada/Salida:
 *   - Lee de stdin usando read() (bajo nivel)
 *   - Escribe a stdout usando write() (bajo nivel)
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
    /* Modos de operación del programa */
    enum { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode = MODE_SUBST;
    
    /* Variables para el modo SQUASH (eliminación de duplicados consecutivos) */
    unsigned char prev = 0;      // Carácter anterior procesado
    int have_prev = 0;            // Flag: ¿hemos procesado algún carácter?

    /* Determinación del modo de operación según los argumentos de línea de comandos */
    if (argc == 1) {
        /* Sin argumentos: modo substitución (espacios -> comas) */
        mode = MODE_SUBST;
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        /* Opción -d: modo eliminación (eliminar espacios) */
        mode = MODE_DELETE;
    } else if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        /* Opción -s: modo squash (eliminar duplicados consecutivos) */
        mode = MODE_SQUASH;
    } else {
        /* Argumentos inválidos: mostrar mensaje de uso y salir */
        fprintf(stderr,
                "Ús:\n  %s            # substitueix ' ' per ','  %s -d         # elimina espais\n  %s -s         # elimina repeticions consecutives\n",
                argv[0], argv[0], argv[0]);
        return 1;
    }

    /* Buffer para leer datos de stdin */
    unsigned char buf[4096];
    ssize_t n;

    /* Bucle principal: leer de stdin, procesar y escribir a stdout */
    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        /* Procesar cada byte leído según el modo seleccionado */
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char c = buf[i];

            if (mode == MODE_SUBST) {
                /* Modo SUBST: sustituir espacios por comas */
                if (c == ' ') c = ',';
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else if (mode == MODE_DELETE) {
                /* Modo DELETE: omitir espacios, escribir el resto */
                if (c == ' ') continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else { // MODE_SQUASH
                /* Modo SQUASH: omitir caracteres consecutivos duplicados */
                if (have_prev && c == prev) continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
                prev = c;
                have_prev = 1;
            }
        }
    }
    
    /* Verificar si hubo error en la lectura */
    if (n < 0) { perror("read"); return 1; }
    
    return 0;
}
