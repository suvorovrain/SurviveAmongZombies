# SurviveAmongZombies
* Build the app via `cmake -S . -B build && cmake --build build`. Then run this binary: `./build/bin/SurviveAmongZombies`.
* Format your code via `scripts/format-*.sh`.
* You can run all CI scripts (build check, formatting checks, lint, leak checks, displaying coverage summary) locally, they are inside `scripts/ci` folder.
* You can receive HTML coverage report like this: `cmake -DENABLE_COVERAGE=ON -S . -B build && cmake --build build && cd build && make coverage`.
