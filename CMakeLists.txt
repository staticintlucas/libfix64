cmake_minimum_required(VERSION 3.16)

project(libfix64
    VERSION 0.1.0
    LANGUAGES C
)

find_package(Python3 3.7 REQUIRED)

set(SCRIPT_DIR ${PROJECT_SOURCE_DIR}/scripts)
set(JINJA_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(GENERATED_DIR ${PROJECT_BINARY_DIR}/src)
file(MAKE_DIRECTORY ${GENERATED_DIR})

message(STATUS "Check for required Python modules")
execute_process(
    COMMAND ${Python3_EXECUTABLE} "requirements.py"
    WORKING_DIRECTORY "${SCRIPT_DIR}"
    RESULT_VARIABLE RESULT
    OUTPUT_VARIABLE OUTPUT
    ERROR_VARIABLE OUTPUT
)
if(RESULT)
    message(FATAL_ERROR ${OUTPUT})
endif()
message(STATUS "Check for required Python modules - done")

set(JINJA_SOURCES
    "fix64.h"
    "fix64_exp.c"
    "fix64_trig.c"
)
set(GENERATED_SOURCES "")

foreach(SRC ${JINJA_SOURCES})
    add_custom_command(
        OUTPUT "${GENERATED_DIR}/${SRC}"
        COMMAND ${Python3_EXECUTABLE} jinja.py
            "${JINJA_SRC_DIR}/${SRC}.jinja" "${GENERATED_DIR}/${SRC}"
        WORKING_DIRECTORY "${SCRIPT_DIR}"
        DEPENDS "${JINJA_SRC_DIR}/${SRC}.jinja"
    )
    list(APPEND GENERATED_SOURCES "${GENERATED_DIR}/${SRC}")
endforeach()

add_library(fix64 STATIC
    ${GENERATED_SOURCES}
)
set_target_properties(fix64 PROPERTIES
    PUBLIC_HEADER "${GENERATED_DIR}/fix64.h"
)
