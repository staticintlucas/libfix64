# 3.12 required for FindPython3 and list(TRANSFORM ...)
cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(libfix64 C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()
if(POLICY CMP0094)
    cmake_policy(SET CMP0094 NEW) # Finds first Python3 on PATH, otherwise breaks CI
endif()
find_package(Python3 3.7 REQUIRED)

set(SCRIPT_DIR ${PROJECT_SOURCE_DIR}/scripts)
set(SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(GENERATED_DIR ${PROJECT_BINARY_DIR}/src)
set(TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/tests)
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

option(CMAKE_WARNINGS_AS_ERRORS "Treat all compile warnings as errors" OFF)
if (CMAKE_WARNINGS_AS_ERRORS)
    add_compile_options(-Werror)
endif()

set(SOURCES
    "fix64_inline.h"
)
set(JINJA_SOURCES
    "fix64.h"
    "fix64_exp.c"
    "fix64_str.c"
    "fix64_trig.c"
)
set(GENERATED_SOURCES "")

list(TRANSFORM SOURCES PREPEND "${SOURCE_DIR}/")
foreach(SRC ${JINJA_SOURCES})
    add_custom_command(
        OUTPUT "${GENERATED_DIR}/${SRC}"
        COMMAND ${Python3_EXECUTABLE} jinja.py
            "${SOURCE_DIR}/${SRC}.jinja" "${GENERATED_DIR}/${SRC}"
        WORKING_DIRECTORY "${SCRIPT_DIR}"
        DEPENDS "${SOURCE_DIR}/${SRC}.jinja"
    )
    list(APPEND GENERATED_SOURCES "${GENERATED_DIR}/${SRC}")
endforeach()

add_library(fix64 STATIC
    ${SOURCES} ${GENERATED_SOURCES}
)
target_include_directories(fix64 PRIVATE "${SOURCE_DIR}")
set_target_properties(fix64 PROPERTIES
    PUBLIC_HEADER "${GENERATED_DIR}/fix64.h"
)

set(TESTS
    sanity consts literals str
    exp exp2 log log2 log10
    sin cos tan
)
set(TEST_LINK_LIBRARIES fix64)

# Link libm if it exists
include(CheckLibraryExists)
check_library_exists(m pow "" NEED_LIBM)
if(NEED_LIBM)
    list(APPEND TEST_LINK_LIBRARIES m)
endif()

foreach(TEST ${TESTS})
    add_executable("test_${TEST}" EXCLUDE_FROM_ALL "${TESTS_SRC_DIR}/${TEST}.c")
    target_include_directories("test_${TEST}" PRIVATE "${GENERATED_DIR}")
    target_link_libraries("test_${TEST}" PRIVATE ${TEST_LINK_LIBRARIES})
    # Use fixture to ensure tests are built
    add_test("build_${TEST}" "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target "test_${TEST}")
    set_tests_properties("build_${TEST}" PROPERTIES FIXTURES_SETUP "fixture_${TEST}")
    add_test("test_${TEST}" "test_${TEST}")
    set_tests_properties("test_${TEST}" PROPERTIES FIXTURES_REQUIRED "fixture_${TEST}")
endforeach()
