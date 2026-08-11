#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv){
    char *out = "y";
    if (argc == 1) goto write;
    size_t tl = 0;
    for (int i = 1; i < argc; i++) tl += strlen(argv[i]);
    out = malloc(tl + argc);
    if (!out) return -1;
    char *current_pos = out;
    for (int i = 1; i < argc; i++){
        if (i != 1) *current_pos++ = ' ';
        size_t len = strlen(argv[i]);
        memcpy(current_pos, argv[i], len);
        current_pos += len;
    }
    *current_pos = '\0';
    write:
    for (;;) puts(out);
}
