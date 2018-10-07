#!/usr/bin/env sh
if [[ `which clang 2>&1` ]]; then
    CC='clang'
else
    CC ='gcc'
fi
${CC} -g -std=c99 tictactoe.c -o tictactoe

