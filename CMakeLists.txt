# 3.12 required for FindPython3 and list(TRANSFORM ...)
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Uses only C code
project(libfix64 C)

# Build options
option(CMAKE_WARNINGS_AS_ERRORS "Treat all compile warnings as errors" OFF)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)
if (CMAKE_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

add_subdirectory(src)
add_subdirectory(tests)
