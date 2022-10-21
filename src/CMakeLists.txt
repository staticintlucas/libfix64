# Source files
set(SOURCES
    "fix64_inline.h"
    "fix64_fallback.c"
)
list(TRANSFORM SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/")

# Jinja2 templates for autoGEN source files
set(JINJA_SOURCES
    "fix64.h"
    "fix64_exp.c"
    "fix64_str.c"
    "fix64_trig.c"
)

# Find Python >= 3.7 for Jinja2 render scripts
if(POLICY CMP0094)
    # Finds first Python3 on PATH, otherwise finds the wrong Python in CI. Oddly the old behaviour
    # (before this policy was introduced) is the same as what the policy calls NEW
    cmake_policy(SET CMP0094 NEW)
endif()
find_package(Python3 3.7 REQUIRED)

# Check for required Python modules
message(STATUS "Check for required Python modules")
execute_process(
    COMMAND ${Python3_EXECUTABLE} "requirements.py"
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
    RESULT_VARIABLE RESULT
    OUTPUT_VARIABLE OUTPUT
    ERROR_VARIABLE OUTPUT
)
if(RESULT)
    message(FATAL_ERROR ${OUTPUT})
endif()
message(STATUS "Check for required Python modules - done")

# Add commands to generate sources
set(GEN_SOURCES "")
foreach(SRC ${JINJA_SOURCES})
    add_custom_command(
        OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SRC}"
        COMMAND ${Python3_EXECUTABLE} jinja.py
            "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.jinja" "${CMAKE_CURRENT_BINARY_DIR}/${SRC}"
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
        DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.jinja"
    )
    list(APPEND GEN_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${SRC}") # Append generated file to list
endforeach()

# Add the library
add_library(fix64 STATIC
    ${SOURCES} ${GEN_SOURCES}
)

# Include the source and generated dirs
target_include_directories(fix64 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(fix64 PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Set public headers
set_target_properties(fix64 PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/fix64.h")
