#!/bin/sh -e
echo "Building the project"
cmake -S . -b build
cmake --build build
echo "OK"
