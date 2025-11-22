#!/bin/sh -e
echo "Format CMake files"
find . -type f \( -name "CMakeLists.txt" \) -not -path "./vendor/*" -print0 | xargs -r -0 cmake-format -i
echo "OK"
