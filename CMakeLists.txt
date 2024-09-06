# Version
cmake_minimum_required( VERSION 3.24.0 )

# Configure Compiler
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )
add_compile_options( "/O2" )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}.. )
set( SOURCES
        "src/main.cpp"
        "src/callback.cpp" )

project( copynpaste )
add_executable( ${PROJECT_NAME} ${SOURCES} )
target_include_directories( ${PROJECT_NAME} PUBLIC "include" )

# target_link_options( ${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS" )

