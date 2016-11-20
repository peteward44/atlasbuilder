#!/bin/bash
# -Wall
mkdir -p target-linux
g++ -o target-linux/atlasbuilder -g main.cpp src/*.cpp -std=c++11 "$@" `pkg-config vips-cpp --cflags --libs` -lboost_program_options -lboost_filesystem -lboost_system

