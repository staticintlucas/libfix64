# add_jinja_template
#
# Add a Jinja2 template to be rendered
#
# Usage
# add_jinja_template(
#     TEMPLATES <file>...
#     OUTPUT_SOURCES <variable>
# )
function(add_jinja_template)
    cmake_parse_arguments(PARSE_ARGV 0 ARG "" "OUTPUT_SOURCES" "TEMPLATES")

    if(DEFINED ARG_UNPARSED_ARGUMENTS)
        list_index(FIRST_UNPARSED 0 ${ARG_UNPARSED_ARGUMENTS})
        message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} called with unknown argument \"${FIRST_UNPARSED}\".")
    endif()
    foreach(ARG IN ITEMS TEMPLATES OUTPUT_SOURCES)
        if(NOT DEFINED ARG_${ARG})
            message(FATAL_ERROR "${CMAKE_CURRENT_FUNCTION} called with no value for ${ARG}.")
        endif()
    endforeach()

    set(GENERATED_SOURCES "")

    foreach(SRC ${ARG_TEMPLATES})
        # Ensure directory exists
        get_filename_component(FILE_DIR "${CMAKE_CURRENT_BINARY_DIR}/${SRC}" DIRECTORY)
        file(MAKE_DIRECTORY ${FILE_DIR})

        # Add build command
        add_custom_command(
            OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/${SRC}"
            COMMAND ${Python3_EXECUTABLE} jinja.py
                "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.jinja" "${CMAKE_CURRENT_BINARY_DIR}/${SRC}"
            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/scripts"
            DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${SRC}.jinja"
        )

        # Append generated file to list
        list(APPEND GENERATED_SOURCES "${CMAKE_CURRENT_BINARY_DIR}/${SRC}")
    endforeach()

    set(${ARG_OUTPUT_SOURCES} ${GENERATED_SOURCES} PARENT_SCOPE)
endfunction()
