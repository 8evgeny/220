#!/usr/bin/bash

cc ball.c -O2 -o ball $(pkg-config --cflags --libs sdl2) -lm


#  sudo apt install libsdl2-dev pkg-config
