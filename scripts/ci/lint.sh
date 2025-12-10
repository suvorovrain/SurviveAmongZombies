#!/bin/sh -e
echo "Lint the code"
rm -rf build
cmake -S . -B build
bear -- cmake --build build
find src -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -r -0 clang-tidy -p
rm -rf build
rm -f compile_commands.json
echo "OK"
