#!/bin/sh -e
echo "Running shellcheck"
find . -type f \( -name "*.sh" \) -print0 | xargs -r -0 shellcheck
echo "OK"
echo "Running shfmt"
find . -type f \( -name "*.sh" \) -print0 | xargs -r -0 shfmt -d .
echo "OK"
