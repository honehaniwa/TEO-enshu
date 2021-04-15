#!/bin/bash
echo compile $1
gcc $1 -I/usr/local/include -L/usr/local/lib -lm -lteo -O2 -Wall -DTEO_DEBUG_ALL
if [ $? = 0 ]; then
    echo run source to ${@:2:($#-1)}
    ./a.out ${@:2:($#-1)}
else
    echo compile Failure
fi