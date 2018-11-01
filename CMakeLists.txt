cmake_minimum_required(VERSION 3.10.2)

project(ALAN_COMPILER
    VERSION 1.0.0
    LANGUAGES C CXX
    )

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ALAN_COMPILER_BIN_DIR "${ALAN_COMPILER_SOURCE_DIR}/bin")
set(ALAN_COMPILER_BUILD_DIR "${ALAN_COMPILER_SOURCE_DIR}/build")
set(ALAN_COMPILER_INCLUDE_DIR "${ALAN_COMPILER_SOURCE_DIR}/src")
set(ALAN_COMPILER_LIB_DIR "${ALAN_COMPILER_SOURCE_DIR}/libs")
set(FLEX_DIR "${ALAN_COMPILER_INCLUDE_DIR}/lexer")
set(BISON_DIR "${ALAN_COMPILER_INCLUDE_DIR}/parser")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${ALAN_COMPILER_BIN_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${ALAN_COMPILER_LIB_DIR}")

# message(${ALAN_COMPILER_SOURCE_DIR})
# message(${ALAN_COMPILER_BIN_DIR})
# message(${ALAN_COMPILER_BUILD_DIR})
# message(${ALAN_COMPILER_INCLUDE_DIR})
# message(${FLEX_DIR})
# message(${BISON_DIR})

add_custom_target(LEXER ALL 
    COMMAND flex -o${FLEX_DIR}/lexer.cpp ${FLEX_DIR}/lexer.l
    )
add_custom_target(PARSER ALL
    COMMAND bison -dv -o ${BISON_DIR}/parser.cpp ${BISON_DIR}/parser.y
    )

file(GLOB_RECURSE SOURCE_FILES
    ${ALAN_COMPILER_SOURCE_DIR}/src/*.cpp
    )

set(SOURCE_FILES ${SOURCE_FILES} ${ALAN_COMPILER_SOURCE_DIR}/main.cpp ${FLEX_DIR}/lexer.cpp ${BISON_DIR}/parser.cpp)
set_source_files_properties(
    ${FLEX_DIR}/lexer.cpp
    ${BISON_DIR}/parser.cpp
    PROPERTIES GENERATED TRUE
    )

add_library(alanstd SHARED ${ALAN_COMPILER_SOURCE_DIR}/src/lib/lib.c)

add_executable(ALAN ${SOURCE_FILES})
add_dependencies(ALAN LEXER PARSER)
target_include_directories(ALAN PRIVATE ${ALAN_COMPILER_INCLUDE_DIR})
target_compile_options(ALAN 
    PRIVATE -Wall
    )
target_link_libraries(ALAN "fl")

add_custom_command(
    TARGET ALAN
    COMMAND cp build/compile_commands.json compile_commands.json
    DEPENDS build/compile_commands.json
    WORKING_DIRECTORY ${ALAN_COMPILER_SOURCE_DIR}
    )
