#!/bin/bash

# Copyright (c) 2021, Wojciech Owczarek
# All rights reserved
# BSD 2-clause licence - see LICENSE in the top directory

# a crude .pas preprocessor to include any {$I xxx.pas} files into Pascal source

if [ "${1}x" = "x" ]; then echo "$0: no source file given"; exit 1; fi
if [ "${2}x" = "x" ]; then echo "$0: no destination file given"; exit 1; fi
if [ ! -f $1 ]; then echo "$0: source file not found"; exit 1; fi

echo -n "Processing includes and minifying $1 into $2..."
awk -F " |}" '{ if (match($0, /{\$I[\t ]+([^\t ]+[pP][aA][sS])}/)) { system("cat "$2);} else print $0; }' $1 |\
  sed 's/{[^$].*}//g' | sed 's/^[ \t]*//g' | sed '/^$/d' | sed 's/[ \t]*$//g' > $2
echo " done"

