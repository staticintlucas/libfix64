# 3.12 required for FindPython3 and list(TRANSFORM ...)
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

# Add include path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(CheckPythonRequirements) # Check for required Python modules
include(AddJinjaTemplate) # Add command to render Jinja templates

# Uses only C code
project(libfix64 C)

# Build options
option(FIX64_WARNINGS_AS_ERRORS "Treat all compile warnings as errors" OFF)
option(FIX64_OVERRIDE_USE_FALLBACK "Use fallback implementations rather than compiler builtins (useful for testing)" OFF)
option(FIX64_EXPORT_COMPILE_COMMANDS "Export a compile_commands.json database" ON)

# Source files
set(SOURCES
    "include/fix64.h"
    "include/fix64/arith.h"
    "include/fix64/cmp.h"
    "include/fix64/impl.h"
    "include/fix64/math.h"
    "include/fix64/str.h"
    "src/fallback.c"
    "src/math/exp.c"
    "src/math/trig.c"
    "src/str.c"
)
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

# Jinja2 templates for autogenerating source files
set(JINJA_SOURCES
    "include/fix64/consts.h"
    "include/fix64/cvt.h"
    "src/math/exp.inc"
    "src/math/trig.inc"
    "src/str.inc"
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
target_include_directories(fix64 PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    "${CMAKE_CURRENT_BINARY_DIR}/src"
)
target_include_directories(fix64 PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_BINARY_DIR}/include"
)

# Compiler options
set_target_properties(fix64 PROPERTIES C_STANDARD 99)
set_target_properties(fix64 PROPERTIES C_EXTENSIONS OFF)
set_target_properties(fix64 PROPERTIES C_STANDARD_REQUIRED ON)
set_target_properties(fix64 PROPERTIES EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Intel")
    target_compile_options(fix64 PRIVATE -Wall -Wextra -Wpedantic)
    if (FIX64_WARNINGS_AS_ERRORS)
        target_compile_options(fix64 PRIVATE -Werror)
    endif()
else()
    message(WARNING "Compiler \"${CMAKE_C_COMPILER_ID}\" not recognised. Continuing without setting flags")
endif()

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (FIX64_OVERRIDE_USE_FALLBACK)
    target_compile_definitions(fix64 PUBLIC FIX64_IMPL_OVERRIDE_USE_FALLBACK)
endif()

# Set public headers
set_target_properties(fix64 PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/fix64.h")

# Add tests
add_subdirectory(tests)
