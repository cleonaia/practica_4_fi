#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    enum { MODE_SUBST, MODE_DELETE, MODE_SQUASH } mode = MODE_SUBST;
    unsigned char prev = 0;
    int have_prev = 0;

    if (argc == 1) {
        mode = MODE_SUBST;         // default: replace '\t' with ','
    } else if (argc == 2 && strcmp(argv[1], "-d") == 0) {
        mode = MODE_DELETE;        // delete spaces ' '
    } else if (argc == 2 && strcmp(argv[1], "-s") == 0) {
        mode = MODE_SQUASH;        // remove consecutive duplicates
    } else {
        fprintf(stderr,
                "Ús:\n  %s            # substitueix ' ' per ','  %s -d         # elimina espais\n  %s -s         # elimina repeticions consecutives\n",
                argv[0], argv[0], argv[0]);
        return 1;
    }

    unsigned char buf[4096];
    ssize_t n;

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        for (ssize_t i = 0; i < n; ++i) {
            unsigned char c = buf[i];

            if (mode == MODE_SUBST) {
                if (c == ' ') c = ',';
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else if (mode == MODE_DELETE) {
                if (c == ' ') continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
            } else { // MODE_SQUASH
                if (have_prev && c == prev) continue;
                if (write(STDOUT_FILENO, &c, 1) != 1) { perror("write"); return 1; }
                prev = c;
                have_prev = 1;
            }
        }
    }
    if (n < 0) { perror("read"); return 1; }
    return 0;
}
