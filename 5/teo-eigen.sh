#!/bin/bash
echo compile $1
g++ $1 -I/usr/local/include -I/usr/local/include/eigen3 -L/usr/local/lib -lm -lteo -O2 -Wall -DTEO_DEBUG_ALL
if [ $? = 0 ]; then
    echo run source to ${@:2:($#-1)}
    ./a.out ${@:2:($#-1)}
else
    echo compile Failure
fi