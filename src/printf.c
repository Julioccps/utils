#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#define PROG "printf"
#define VERSION_CODE "0.0.1"
#define USAGE "fomart [arguments]\n" \
    "Formats and prints the formatted string and its arguments\n"\
    "Option:\n" \
    "\t--help, -h\n" \
	"\t\tShows this message and exits\n" \
	"\t--version, -v\n" \
	"\t\tShows version info and exits\n"

#define BINARY_IO
#define COMMON_IMPLEMENTATION
#include <common.h>

int main(int argc, char **argv){
    prepare_binary_io();
    if (argc < 2){
        usage();
        return SUCCESS;
    }
    for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
			usage();
			return SUCCESS;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
			version();
			return SUCCESS;
		}
    }
    const char *format = argv[1];
    int arg_idx = 2;
    for (const char *p = format; *p != '\0'; p++){
        if (*p == '\\'){
            p++;
            switch (*p){
                case 'n': putchar('\n'); break;
                case 't': putchar('\t'); break;
                case '\\': putchar('\\'); break;
                default: putchar('\\'); putchar(*p); break;
            }
            continue;
        }
        if (*p != '%'){
            putchar(*p);
            continue;
        }
        p++;
        const char *arg = arg_idx < argc? argv[arg_idx++]: "";

        switch (*p){
            case 's':
                printf("%s", arg);
                break;
            case 'd':
            case 'i':
                long long ll_val = strtoll(arg, NULL, 0);
                printf("%lld", ll_val);
                break;
            case 'x': {
                unsigned long long x_val = strtoull(arg, NULL, 0);
                printf("%llx", x_val);
                break;
            }
            case 'X': {
                unsigned long long X_val = strtoull(arg, NULL, 0);
                printf("%llX", X_val);
                break;
            }
            case 'f':
                double d_val = strtod(arg, NULL);
                printf("%f", d_val);
                break;
            case '%':
                putchar('%');
                break;
            default:
                putchar('%');
                putchar(*p);
                break;
        }
    }
}
