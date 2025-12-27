# SurviveAmongZombies
Vampire survivors-like 2D game with isometric graphic written with C and based on [Glebas Engine](https://github.com/Sibiri4ok/GTA-VI)
## Development dependencies
* `sudo apt install build-essentials`
* `sudo apt install cmake`
* `sudo apt install shellcheck`
* `sudo apt install shfmt`
* `sudo apt install bear`
* `sudo apt install clang-format-20`
* `sudo apt install clang-tidy`
* `pip install cmakelang`
## Development notices
* Build the app via `cmake -S . -B build && cmake --build build`. Then run this binary: `./build/bin/SurviveAmongZombies`.
* Format your code via `scripts/format-*.sh`.
* You can run all CI scripts (build check, formatting checks, lint) locally, they are inside `scripts/ci` folder.

