#!/bin/bash

bps=90

which unix2dos >/dev/null || { echo "`basename $0`: missing 'unix2dos', exiting."; exit -1; }
which pv >/dev/null || { echo "`basename $0`: missing 'pv', exiting."; exit -1; }

if [ ! -f "$1" ]; then
echo "No file specified or file not found. Usage: `basename $0` <file> <serial device>"
exit 1
fi

if [ "${2}x"=="x" ]; then dev=/dev/ttyUSB0; else dev=$2; fi
stty -F $dev min 1 time 0 speed 9600 >/dev/null 2>&1

fs=`wc -c $1 | awk '{print $1;}'`
cfs=$( cat $1 | sed 's/{[^$].*}//g' | sed 's/^[ \t]*//g' | sed '/^$/d' | sed 's/[ \t]*$//g' | wc -c )
echo "PB-2000 should be loading the file first (MENU->[load]->[rs232c]->7N81NNN.NN->EXE)"
echo "Sending file '$1' to $dev ($fs bytes, reduced to $cfs) at $bps Bps..."
cat $1 | sed 's/{[^$].*}//g' | sed 's/^[ \t]*//g' | sed '/^$/d' | sed 's/[ \t]*$//g' | unix2dos | pv -b -C -p -t -e -r -b -i 1 -L $bps >$dev
echo "Done. Press BRK then any key on PB to complete transfer."
