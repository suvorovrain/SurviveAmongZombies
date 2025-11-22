#!/bin/sh -e
echo "Check CMake files formatting"
find . -type f \( -name "CMakeLists.txt" \) -not -path "./vendor/*" -print0 | xargs -r -0 cmake-format --check
echo "OK"
echo "Lint CMake files"
find . -type f \( -name "CMakeLists.txt" \) -not -path "./vendor/*" -print0 | xargs -r -0 cmake-lint
echo "OK"
