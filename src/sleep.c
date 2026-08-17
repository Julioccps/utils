#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <windows.h>

#define PROG "sleep"
#define VERSION_CODE "0.0.1"
#define USAGE "[option] | NUMBER[SUFIX]\n"\
    "Pause for NUMBER of (SUFIX), SUFIX may be 's' (seconds, default)\n" \
    "'m' for minutes, 'h' for hours and 'd' for days\n" \
    "Option:\n" \
	"\t--help, -h\n" \
	"\t\tShows this message and exits\n" \
	"\t--version, -v\n" \
	"\t\tShows version info and exits\n"

#define COMMON_IMPLEMENTATION
#include <common.h>

int parse_time(const char *time) {
    char *end;
    long value = strtol(time, &end, 10);

    if (end == time) {
        return -1;
    }

    int mul = 1000;
    switch (*end) {
        case 's': mul *= 1;         break;
        case 'm': mul *= 60;        break;
        case 'h': mul *= 3600;      break;
        case 'd': mul *= 86400;     break;
        case '\0': mul *= 1;        break;
        default:
            return -1;
    }

    if (*end != '\0' && *(end + 1) != '\0') {
        return -1;
    }

    return (int)(value * mul);
}

int main(int argc, char **argv){
    if (argc < 2){
        usage();
        return SUCCESS;
    }
    int sleepms = 0;
    for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
			usage();
			return SUCCESS;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
			version();
			return SUCCESS;
		}
		else {
		  int ms = parse_time(argv[i]);
		  /* Sleep() takes an unsigned DWORD: a negative value here would wrap
		   * to a huge positive one (-1 becomes INFINITE) instead of failing.
		   */
		  if (ms < 0){
		      fputs(PROG ": invalid time: ", stderr);
		      fputs(argv[i], stderr);
		      putc('\n', stderr);
		      return EX_USAGE;
		  }
		  sleepms += ms;
		}
    }
    Sleep((DWORD)sleepms);
    return 0;
}
