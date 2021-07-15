#!/bin/bash

# Copyright (c) 2021, Wojciech Owczarek
# All rights reserved
# BSD 2-clause licence - see LICENSE in the top directory


# this is a simple script to send a (text!) file to a serial device,
# at a specific baud rate, with throughput limited to approx. n Bps,
# using a fixed delay after every byte. The actual throughput varies
# and will be less than requested, but never more :-)

# defaults
bps=0
dev=/dev/ttyUSB0

if [ ! -f "$1" ]; then
    echo "No file specified or file not found. Usage: `basename $0` <file> <serial device> <Bps limit (default:0 = no limit)>"
    exit 1
fi

# crude, just the way I like it
if [ "${2}x" != "x" ]; then dev=$2; fi
if [ "${3}x" != "x" ]; then bps=$3; fi

# who says we can't do float in Bash!
if [ $bps -gt 0 ]; then delay=`printf "0.%03d" $(( 1000 / $bps ))`; fi

# set desired speed and make sure we don't have min==time==0, otherwise writes will end prematurely
# TODO: baud rate

stty -F $dev min 1 time 0 speed 9600 >/dev/null 2>&1

# "unix2dos": first replace any \r\n with \n, then back to \r\n to ensure no mixed newline types or doubles
function u2d {
    sed -e ':a' -e 'N' -e '$!ba' -e 's/\r\n/\n/g' -e 's/\n/\r\n/g'
}

# Far from a real minify, but remove any comments apart from preprocessor {$xxx},
# any pre/post indentation and any empty newlines. We are not removing newlines
# completely (which could gain 2 bytes per line), because DL-Pascal has issues
# with some very long strings like the inline() calls.
function pminify {
    sed 's/{[^$].*}//g' | sed 's/^[ \t]*//g' | sed '/^$/d' | sed 's/[ \t]*$//g'
}

fs=`cat $1 | u2d | wc -c | awk '{print $1;}'`
cfs=$( cat $1 | pminify | u2d | wc -c )
echo "PB-2000 should be loading the file first (MENU->ETC.->[load]->[rs232c]->7N81NNN.NN->EXE)"

str="$fs bytes"
if [ $fs -gt $cfs ]; then str="$str, reduced to $cfs"; else str="$str, could not reduce further"; fi

sbps=""
if [ $bps -gt 0 ]; then sbps=" at ~$bps Bps"; fi
echo "Sending file '$1' to $dev ($str)$sbps:"

pdone=0
pdone1=-1
cnt=0
cat $1 | pminify | u2d | {
    # read byte after byte
    while read -N 1 -d '' c; do
        cnt=$(( $cnt + 1))
        # the old trick: round the percentage down to full tens...
        pdone=$(( 10 * ((10 * $cnt) / $cfs) ))
        # ...and only print if ticked one up, hence initialised pdone1 to less than zero
        if [ "$pdone" -gt "$pdone1" ]; then echo -n $pdone%...;fi >&2

        # byte out
        echo -n "$c"

        # extra delay
        if [ $bps -gt 0 ]; then
            # delay option 1: coproc reading from an stdin that is guaranteed not to receive anything
            # { coproc read -t $delay; wait $!; } >/dev/null 2>&1

            # delay option 2, more precise, but can potentially mess up stderr
            read -t $delay </dev/stderr
        fi
    pdone1=$pdone
    done
} > $dev

echo
echo "Done. Press BRK then any key on PB to complete transfer."
