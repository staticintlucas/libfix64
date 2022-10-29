# 3.12 required for FindPython3 and list(TRANSFORM ...)
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Add include path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CheckPythonRequirements) # Check for required Python modules
include(AddJinjaTemplate) # Add command to render Jinja templates

# Uses only C code
project(libfix64 C)

# Build options
option(CMAKE_WARNINGS_AS_ERRORS "Treat all compile warnings as errors" OFF)

# Compiler options
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

# Source files
set(SOURCES
    "src/fix64_inline.h"
    "src/fix64_fallback.c"
)
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

# Jinja2 templates for autogenerating source files
set(JINJA_SOURCES
    "src/fix64.h"
    "src/fix64_exp.c"
    "src/fix64_str.c"
    "src/fix64_trig.c"
)
add_jinja_template(
    TEMPLATES ${JINJA_SOURCES}
    OUTPUT_SOURCES GEN_SOURCES
)

# Add the library
add_library(fix64 STATIC
    ${SOURCES} ${GEN_SOURCES}
)

# Include the source and generated dirs
target_include_directories(fix64 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src")
target_include_directories(fix64 PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/src")

# Set public headers
set_target_properties(fix64 PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/fix64.h")

# Add tests
add_subdirectory(tests)
