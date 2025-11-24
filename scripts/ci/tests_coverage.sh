#!/bin/sh -e
echo "Running tests"
cmake -DENABLE_COVERAGE=ON -S . -B build
cmake --build build
cd build
make coverage-summary
cd ..
rm -rf build
echo "OK"
