#!/bin/sh -e
echo "Format the code"
find src -type f \( -name "*.c" -o -name "*.h" \) -print0 | xargs -r -0 clang-format-20 -i
echo "OK"
