#!/bin/sh

# CMake seems to always generate the compiler command such that the last 4
# arguments are "-o <output> -c <input>"

eval "IN=\${$#}"              # Extract "<input>"
eval "OUT=\${$(($# - 2))}"    # Extract "<output>"

# Rebuild COMPILE command with last 4 args stripped away
i=1
COMPILE=""
while [ $i -le $(($# - 4)) ]; do
    eval "arg=\${$i}"
    COMPILE="$COMPILE \"$arg\""
    i=$(($i + 1))
done

eval "$COMPILE -S -o- \"$IN\" \
    | ./InstrumentInstructionCallback \
    | $COMPILE -o \"$OUT\" -xassembler -c -"

