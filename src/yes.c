#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUF_SIZE (64u * 1024u)
#define PROG "yes"
#define VERSION_CODE "0.0.5"
#define USAGE "[option] | [STRING]...\n" \
    "Option:\n" \
	"\t--help, -h\n" \
	"\t\tShows this message and exits\n" \
	"\t--version, -v\n" \
	"\t\tShows version info and exits\n" \
	"Repeatedly outputs a line with all the STRINGs, or 'y'"
#define BINARY_IO
#define COMMON_IMPLEMENTATION
#include <common.h>

int main(int argc, char **argv){
    prepare_binary_io();
    const char *line = "y\n";
    size_t line_len = 2;
    char *allocated = NULL;

    if (argc == 2){
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0){
            usage();
            return SUCCESS;
        }
        else if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0){
            version();
            return SUCCESS;
        }
    }

    if (argc > 1){
        size_t tl = 0;
        for (int i = 1; i < argc; i++) tl += strlen(argv[i]);
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

    setvbuf(stdout, NULL, _IONBF, 0);
    while (fwrite(buf, 1, fill, stdout) == fill);

    free(buf);
    free(allocated);
    return EXIT_FAILURE;
}
