#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE (64u * 1024u)

int main(int argc, char **argv){
    const char *line = "y\n";
    size_t line_len = 2;
    char *allocated = NULL;

    if (argc > 1){
        size_t tl = 0;
        for (int i = 1; i < argc; i++) tl += strlen(argv[i]);
        /* the words, (argc - 2) spaces between them, and one '\n' */
        line_len = tl + (size_t)argc - 1;
        allocated = malloc(line_len);
        if (!allocated) return EXIT_FAILURE;
        char *current_pos = allocated;
        for (int i = 1; i < argc; i++){
            if (i != 1) *current_pos++ = ' ';
            size_t len = strlen(argv[i]);
            memcpy(current_pos, argv[i], len);
            current_pos += len;
        }
        *current_pos = '\n';
        line = allocated;
    }

    /* tile the line into a big buffer so one write covers many lines */
    size_t buf_len = BUF_SIZE < line_len ? line_len : BUF_SIZE;
    char *buf = malloc(buf_len);
    if (!buf){
        free(allocated);
        return EXIT_FAILURE;
    }
    size_t fill = 0;
    while (fill + line_len <= buf_len){
        memcpy(buf + fill, line, line_len);
        fill += line_len;
    }

    /* unbuffered: stop stdio from copying our buffer through its own */
    setvbuf(stdout, NULL, _IONBF, 0);
    while (fwrite(buf, 1, fill, stdout) == fill);

    free(buf);
    free(allocated);
    return EXIT_FAILURE;
}
