#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PROG "seq"
#define VERSION_CODE "0.0.2"
#define USAGE "[option] last| first last | first step last\n \
    Option:\n \
    \t--help, -h\n \
    \t\tShows this message and exits\n \
    \t--version, -v\n \
    \t\tShows version info and exits\n \
    \t--equal-width, -w\n \
    \t\tequalizes width with largest number with zeros\n \
    \t--separator STRING, -s STRING\n \
    \t\tuse STRING as the separator, default: '\\n'"

#define COMMON_IMPLEMENTATION
#include <common.h>


int isnumber(const char *s){
	if (*s == '-' || *s == '+') s++;
	return isdigit((unsigned char)*s);
}

int main(int argc, char **argv){
	if (argc == 1) {
		usage();
		return 0;
	}
	int first = 1;
	int increment = 1;
	int last = 0;
	char *separator = "\n";
	unsigned char equal_width = 0;
	int nums[3];
	int count = 0;
	for (int i = 1; i < argc; i++){
		if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0){
			usage();
			return 0;
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0){
			version();
			return 0;
		}
		else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--separator") == 0){
			if (++i >= argc) {
				fputs("Not Enough arguments\n", stderr);
				return -1;
			}
			separator = argv[i];
		}
		else if (strcmp(argv[i], "-w") == 0 || strcmp(argv[i], "--equal-width") == 0) equal_width = 1;
		else if (isnumber(argv[i])){
			if (count == 3){
				fputs("Too many arguments\n", stderr);
				return -1;
			}
			char *end;
			nums[count++] = strtol(argv[i], &end, 10);
			if (*end != '\0'){
				fprintf(stderr, "Invalid number: %s\n", argv[i]);
				return -1;
			}
		}
		else {
			fprintf(stderr, "Unknown option: %s\n", argv[i]);
			return -1;
		}
	}

	switch (count){
		case 1: last = nums[0]; break;
		case 2: first = nums[0]; last = nums[1]; break;
		case 3: first = nums[0]; increment = nums[1]; last = nums[2]; break;
		default:
			fputs("Not Enough arguments\n", stderr);
			return -1;
	}

	if (increment == 0){
		fputs("Increment cannot be zero\n", stderr);
		return -1;
	}

	int width = 1;
	if (equal_width){
		int a = snprintf(NULL, 0, "%d", first);
		int b = snprintf(NULL, 0, "%d", last);
		width = a > b? a: b;
	}

	unsigned char printed = 0;
	for (int i = first; increment > 0? i <= last: i >= last; i += increment){
		if (printed) fputs(separator, stdout);
		printf("%0*d", width, i);
		printed = 1;
	}
	if (printed) putchar('\n');
	return 0;
}
