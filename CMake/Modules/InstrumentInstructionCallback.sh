#!/bin/sh

# CMake seems to always generate the compiler command such that the last 4
# arguments are "-o <output> -c <input>"

IN="${@: -1:1}"              # Extract "<input>"
OUT="${@: -3:1}"             # Extract "<output>"
COMPILE="${@:1:$(($#-4))}"   # Extract all arguments before the last 4

$COMPILE -S -o- "$IN" \
    | ./InstrumentInstructionCallback \
    | $COMPILE -o "$OUT" -xassembler -c -
