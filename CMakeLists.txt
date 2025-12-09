cmake_minimum_required(VERSION 3.22)
project(SurviveAmongZombies C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set(PLATFORM_WINDOWS TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  set(PLATFORM_LINUX TRUE)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
  set(PLATFORM_MACOS TRUE)
endif()

option(ENABLE_COVERAGE "Enable code coverage" OFF)
if(ENABLE_COVERAGE)
  add_compile_options(-fprofile-arcs -ftest-coverage)
  add_link_options(-fprofile-arcs -ftest-coverage)
endif()

set(SRC_COMPILE_OPTIONS -O2 -Wall -Wextra -pedantic)
set(VENDOR_COMPILE_OPTIONS -O2 -w)

add_library(stb STATIC vendor/stb/stb_implementations.c)

target_include_directories(stb PUBLIC ${CMAKE_SOURCE_DIR}/vendor/stb)
target_compile_options(stb PRIVATE ${VENDOR_COMPILE_OPTIONS})

file(GLOB_RECURSE GTA_VI_SOURCES vendor/GTA-VI/src/*.c)
add_library(gta_vi OBJECT ${GTA_VI_SOURCES})

target_include_directories(
  gta_vi
  PUBLIC ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/include
         ${CMAKE_SOURCE_DIR}/vendor/GTA-VI/src ${CMAKE_SOURCE_DIR}/vendor/stb)

target_compile_options(gta_vi PRIVATE ${VENDOR_COMPILE_OPTIONS})

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

find_package(PkgConfig QUIET)
pkg_check_modules(SDL2_TTF QUIET sdl2_ttf)

if(NOT SDL2_TTF_FOUND)
  find_path(
    SDL2_TTF_INCLUDE_DIRS SDL_ttf.h
    PATHS /usr/include /usr/local/include
    PATH_SUFFIXES SDL2)

  find_library(
    SDL2_TTF_LIBRARIES
    NAMES SDL2_ttf
    PATHS /usr/lib /usr/local/lib)

  if(SDL2_TTF_INCLUDE_DIRS AND SDL2_TTF_LIBRARIES)
    set(SDL2_TTF_FOUND TRUE)
    message(STATUS "Found SDL2_ttf manually: ${SDL2_TTF_LIBRARIES}")
  else()
    message(FATAL_ERROR "SDL2_ttf not found! Please install libsdl2-ttf-dev")
  endif()
else()
  message(STATUS "Found SDL2_ttf via pkg-config")
endif()

find_library(MATH_LIBRARY m)

add_subdirectory(src)
add_subdirectory(tests)
