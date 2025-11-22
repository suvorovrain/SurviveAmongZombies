#!/bin/sh -e
echo "Building the project"
cmake -S . -B build
cmake --build build
rm -rf build
echo "OK"
