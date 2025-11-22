#!/bin/sh -e
echo "Running tests"
cmake -S . -B build
cmake --build build
./build/bin/tests/SurviveAmongZombies_tests
rm -rf build
echo "OK"
