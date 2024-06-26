# Version
cmake_minimum_required( VERSION 3.20.0 )

# Configure Compiler
set( CMAKE_CXX_COMPILER "C:/mingw64/bin/g++.exe" )
set( CMAKE_CXX_STANDARD 17 )
set( CMAKE_CXX_STANDARD_REQUIRED True )

set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}.. )
set( SOURCES
        "src/main.cpp"
        "src/hotkey.cpp"
        "src/callback.cpp"
        "src/utils.cpp" )

project( copynpaste )
add_executable( ${PROJECT_NAME} ${SOURCES} )

target_include_directories( ${PROJECT_NAME} PUBLIC "include" )

