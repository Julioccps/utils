NAME
    \Sprintf\c - prints formatted strings

USAGE
    \Sprintf\c FORMAT [arguments]

OPTION
    --help, -h
	    Shows this message and exits
	--version, -v
	    Shows version info and exits

NOTE
    An unrecognized \\X sequence is printed back verbatim, backslash
    included, so a Windows path like \SC:\dev\\c\utils\c can usually be
    typed with single backslashes as-is. This breaks if a path segment
    starts with 't' or 'n' right after a backslash (\Stemp\c, \Snew\c):
    those are read as tab and newline, same as they would be in the
    format string of any other printf. Double the backslash there.

EXAMPLE
    $ printf "%x\n" 123 # prints 123 in hex
    $ printf "%s\n" "C:\dev\\c\utils"     # works as typed
    $ printf "%s\n" "C:\\Users\\me\\temp" # needs doubling, has \t
