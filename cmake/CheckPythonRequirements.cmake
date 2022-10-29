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
