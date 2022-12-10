enable_testing()

# List all tests
set(TESTS
    sanity
    consts
    literals
    str
    exp
    exp2
    log
    log2
    log10
    sin
    cos
    tan
)
if(FIX64_OVERRIDE_USE_FALLBACK)
    list(APPEND TESTS
        fallback_overflow
        fallback_int128
        fallback_clz
    )
endif()

# All tests link to libfix64 of course
set(TEST_LINK_LIBRARIES fix64)

# Link to libm if it exists, otherwise assume math functions are in libc
include(CheckLibraryExists)
check_library_exists(m pow "" NEED_LIBM)
if(NEED_LIBM)
    list(APPEND TEST_LINK_LIBRARIES m)
endif()

foreach(TEST ${TESTS})
    # Add test executabe, but don't build when running make all
    add_executable("test_${TEST}" EXCLUDE_FROM_ALL "${CMAKE_CURRENT_SOURCE_DIR}/${TEST}.c")
    target_link_libraries("test_${TEST}" PRIVATE ${TEST_LINK_LIBRARIES})

    # Compile options
    set_target_properties("test_${TEST}" PROPERTIES C_STANDARD 99)
    set_target_properties("test_${TEST}" PROPERTIES C_EXTENSIONS OFF)
    set_target_properties("test_${TEST}" PROPERTIES C_STANDARD_REQUIRED ON)
    set_target_properties("test_${TEST}" PROPERTIES EXPORT_COMPILE_COMMANDS ON)
    if(CMAKE_C_COMPILER_ID MATCHES "Clang" OR CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID STREQUAL "Intel")
        target_compile_options("test_${TEST}" PRIVATE -Wall -Wextra -Wpedantic)
        if (FIX64_WARNINGS_AS_ERRORS)
            target_compile_options("test_${TEST}" PRIVATE -Werror)
        endif()
    else()
        message(WARNING "Compiler \"${CMAKE_C_COMPILER_ID}\" not recognised. Continuing without setting flags")
    endif()

    # Add a "test" which builds the actual test binary. Add as setup for each test's fixture
    add_test("build_${TEST}" "${CMAKE_COMMAND}" --build ${CMAKE_BINARY_DIR} --target "test_${TEST}")
    set_tests_properties("build_${TEST}" PROPERTIES FIXTURES_SETUP "fixture_${TEST}")

    # Add each test, requiring the fixture to build the test
    add_test("test_${TEST}" "test_${TEST}")
    set_tests_properties("test_${TEST}" PROPERTIES FIXTURES_REQUIRED "fixture_${TEST}")
endforeach()
