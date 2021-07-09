This is the source of the HD61700 cross-assembler rev 0.44 by Blue (http://hd61700.yukimizake.net/).

Version 0.44 is 0.43 with my contributions:

	- Replaced all "unsigned long" with "unsigned int" to keep format strings and compilers happy and build on Linux
	- Support for DL-Pascal's inline() output (/s flag + __DLP_INLINE build flag)
	- Reset return code to 0 if EOF error is returned (which it usually is), to keep make from barfing out
