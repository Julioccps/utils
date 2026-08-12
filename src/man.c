#define WIN32_LEAN_AND_MEAN     // keeps windows.h from defining 'small' and friends

#include <io.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdio.h>

#define PROG "man"
#define VERSION_CODE "0.0.1"
#define USAGE "[option] | <program_name>\n" \
    "Option:\n" \
    "\t--help, -h\n" \
	"\t\tShows this message and exits\n" \
	"\t--version, -v\n" \
	"\t\tShows version info and exits\n" \
	"Shows the manual for selected program"

#define COMMON_IMPLEMENTATION
#include <common.h>

#define FILE_EXT ".dm"  // Document for my Man

/* Points straight at the folder holding the pages, not at the install root.
 * It exists so the tool can be tested from build/ without installing.
 */
#define ENV_MANPATH "UTILS_MANPATH"
#define MAN_SUBDIR  "\\man\\"   // appended to the exe folder when the env is unset

#define SGR_BOLD  "\033[1m"
#define SGR_UNDER "\033[4m"
#define SGR_RESET "\033[0m"

static const struct { char mark; const char *ansi; } marks[] = {
    {'B', SGR_BOLD},
    {'S', SGR_UNDER},
    {'c', SGR_RESET},
};

/* Writes buf to stream, turning the markers into ANSI when tty is set and
 * dropping them when it is not. A line starting at column zero is a section
 * heading and gets bold on its own, no markup needed.
 * Markers add up: \B then \S is bold and underlined. \c clears everything,
 * there is no way to turn off just one of them, on purpose.
 */
static void display_dm(const char *buf, FILE *stream, int tty){
    int line_start = 1;
    int heading = 0;

    while (*buf){
        if (line_start){
            line_start = 0;
            if (*buf != ' ' && *buf != '\t' && *buf != '\n' && *buf != '\r'){
                heading = 1;
                if (tty) fputs(SGR_BOLD, stream);
            }
        }

        switch (*buf){
            case '\n':
                if (heading){
                    if (tty) fputs(SGR_RESET, stream);
                    heading = 0;
                }
                putc(*buf++, stream);
                line_start = 1;
                break;

            case '\\':
                if (buf[1] == '\0'){        // trailing backslash, nothing to peek
                    putc(*buf++, stream);
                    break;
                }
                if (buf[1] == '\\'){
                    putc('\\', stream);
                    buf += 2;
                    break;
                }
                {
                    const char *ansi = NULL;
                    for (size_t i = 0; i < sizeof marks / sizeof marks[0]; i++){
                        if (marks[i].mark == buf[1]){
                            ansi = marks[i].ansi;
                            break;
                        }
                    }
                    if (ansi){
                        if (tty) fputs(ansi, stream);
                    }
                    else {                  // unknown marker, leave it alone
                        putc(buf[0], stream);
                        putc(buf[1], stream);
                    }
                    buf += 2;
                }
                break;

            default:
                putc(*buf++, stream);
                break;
        }
    }

    if (heading && tty) fputs(SGR_RESET, stream);
}

/* Fills dir with the folder holding the running exe, without a trailing slash.
 * Returns 0 if the call failed or if the path did not fit: GetModuleFileNameA
 * signals truncation by returning the buffer size instead of the length.
 * argv[0] is not usable here, whoever spawns us controls it.
 */
static int exe_dir(char *dir, size_t size){
    DWORD n = GetModuleFileNameA(NULL, dir, (DWORD)size);
    if (n == 0 || n == size) return 0;
    char *slash = strrchr(dir, '\\');   // always backslashes, never '/'
    if (!slash) return 0;
    *slash = '\0';
    return 1;
}

/* Joins base + sub + name + FILE_EXT into out. Returns 0 if it would not fit.
 * Done by hand instead of snprintf so man keeps no formatting machinery.
 */
static int join_path(char *out, size_t size, const char *base,
                     const char *sub, const char *name){
    size_t lb = strlen(base), ls = strlen(sub);
    size_t ln = strlen(name), le = strlen(FILE_EXT);

    if (lb + ls + ln + le + 1 > size) return 0;
    memcpy(out, base, lb);
    memcpy(out + lb, sub, ls);
    memcpy(out + lb + ls, name, ln);
    memcpy(out + lb + ls + ln, FILE_EXT, le + 1);   // +1 carries the NUL
    return 1;
}

static void err_path(const char *msg, const char *name, const char *path){
    fputs(PROG ": ", stderr);
    fputs(msg, stderr);
    fputs(name, stderr);
    fputs(", looked in ", stderr);
    fputs(path, stderr);
    putc('\n', stderr);
}

int main(int argc, char **argv){
    if (argc < 2){
        usage();
        return 0;
    }

    const char *name = NULL;
    for (int i = 1; i < argc; i++){
        if (strcmp(argv[i], HELP_OPS) == 0 || strcmp(argv[i], HELP_OPL) == 0){
            usage();
            return 0;
        }
        else if (strcmp(argv[i], VERSION_OPS) == 0 || strcmp(argv[i], VERSION_OPL) == 0){
            version();
            return 0;
        }
        else if (name){
            fputs(PROG ": one manual at a time\n", stderr);
            return EX_USAGE;
        }
        else name = argv[i];
    }

    if (!name){
        usage();
        return EX_USAGE;
    }
    if (strpbrk(name, "\\/:")){
        fputs(PROG ": ", stderr);
        fputs(name, stderr);
        fputs(" is a name, not a path\n", stderr);
        return EX_USAGE;
    }

    char dir[MAX_PATH];
    char path[MAX_PATH];
    const char *base = getenv(ENV_MANPATH);
    const char *sub = "\\";     // the env var points at the pages folder itself

    if (!base){
        if (!exe_dir(dir, sizeof dir)){
            fputs(PROG ": could not locate my own folder\n", stderr);
            return EXIT_FAILURE;
        }
        base = dir;
        sub = MAN_SUBDIR;
    }

    if (!join_path(path, sizeof path, base, sub, name)){
        fputs(PROG ": resulting path is too long\n", stderr);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(path, "r");     // text mode, so CRLF collapses on read
    if (!f){
        err_path("no manual for ", name, path);
        return EXIT_FAILURE;
    }

    if (fseek(f, 0, SEEK_END) != 0){
        err_path("could not read the manual for ", name, path);
        fclose(f);
        return EXIT_FAILURE;
    }
    long size = ftell(f);
    rewind(f);
    if (size < 0){
        err_path("could not read the manual for ", name, path);
        fclose(f);
        return EXIT_FAILURE;
    }

    char *buf = malloc((size_t)size + 1);
    if (!buf){
        fclose(f);
        return EXIT_FAILURE;
    }
    size_t n = fread(buf, 1, (size_t)size, f);  // text mode reads fewer than size
    buf[n] = '\0';
    fclose(f);

    int tty = _isatty(_fileno(stdout));
    if (tty){
        HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        if (GetConsoleMode(h, &mode))
            SetConsoleMode(h, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    display_dm(buf, stdout, tty);
    free(buf);
    return 0;
}
