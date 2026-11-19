#!/usr/bin/bash

cc drone.c -O2 -o drone $(pkg-config --cflags --libs sdl2) -lm
cc ball.c -O2 -o ball $(pkg-config --cflags --libs sdl2) -lm

#  sudo apt install libsdl2-dev pkg-config
