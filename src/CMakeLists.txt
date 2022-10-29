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

# Check for required Python modules
include(CheckPythonRequirements)

# Add commands to generate sources
include(AddJinjaTemplate)
add_jinja_template(
    TEMPLATES ${JINJA_SOURCES}
    OUTPUT_SOURCES GEN_SOURCES
)

# Add the library
add_library(fix64 STATIC
    ${SOURCES} ${GEN_SOURCES}
)

# Include the source and generated dirs
target_include_directories(fix64 PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(fix64 PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")

# Set public headers
set_target_properties(fix64 PROPERTIES PUBLIC_HEADER "${CMAKE_CURRENT_BINARY_DIR}/fix64.h")
