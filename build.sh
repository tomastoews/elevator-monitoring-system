#!/bin/bash

gcc main.c \
        -o program \
        -I/usr/include/SDL2 \
        -L/usr/lib \
        -lSDL2main \
        -lSDL2 \
        -lSDL2_ttf

