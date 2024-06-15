#!/bin/bash
g++ -c src/*.cpp
g++ *.o -o Game -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
