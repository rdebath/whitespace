#!/bin/sh -

{
    # awk -f wsc.awk "$1" > /tmp/_wsc.c &&
    ./ws2c -c "$1" > /tmp/_wsc.c &&
    gcc -c -O3 $2 -o /tmp/_wsc.o /tmp/_wsc.c &&
    gcc    -O3 $3 -o /tmp/_wsc /tmp/_wsc.o
} || exit
# rm -f /tmp/_wsc.c ||:
rm -f /tmp/_wsc.o ||:
exec /tmp/_wsc
