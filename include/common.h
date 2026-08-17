#ifndef COMMON_H
#define COMMON_H

#define HELP_OPL    "--help"
#define HELP_OPS    "-h"
#define VERSION_OPL "--version"
#define VERSION_OPS "-v"

/* Exit status for bad usage: wrong options, wrong argument count, etc.
 * Must be a small positive number. Returning -1 makes the shell see
 * 255, 127 or -1 depending on who is asking, and 127 in particular
 * means "command not found" to any script checking $?.
 */
#define EX_USAGE 1
#define SUCCESS  0
typedef enum{
    OP_NOARG,   // No arguments required
    OP_RARG     // Argument is required
} option_type_t;

typedef struct {
    /* Decision:
     * The logic of default values should be specific to each
     * implementation, same for the value extraction. It is better
     * to store here as a raw string and deal with it there, than
     * to try and fix every possible problem in a single place.
     */
    const char *opl;
    const char *ops;
    char *val;      // In an OP_NOARG leave it uninitialized
    option_type_t ot;
} option_t;

#endif

#ifdef COMMON_IMPLEMENTATION
#ifndef COMMON_IMPLEMENTED
#define COMMON_IMPLEMENTED

#include <stdio.h>

#if !defined(PROG) || !defined(VERSION_CODE) || !defined(USAGE)
#error "Each tool needs to define PROG, VERSION_CODE and USAGE"
#endif

static inline void usage(void){
    fputs(PROG " " USAGE "\n", stdout);
}

static inline void version(void){
    fputs(PROG " version: " VERSION_CODE "\n", stdout);
}

#endif
#endif
