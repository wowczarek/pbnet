This is the source of the HD61700 cross-assembler rev 0.43 by Blue (http://hd61700.yukimizake.net/).

The following necessary modifications have been made:

	- Made it compile on contemporary compilers with correct int size ( "unsigned long" vs "unsigned int" )
	- Added support for DL-Pascal's inline() output (/s flag)
	- Reset return code to 0 if EOF error is returned (which it usually is), to keep make from barfing out

Otherwise, no modifications to the core operation of the compiler have been made and the original readme files are included.
