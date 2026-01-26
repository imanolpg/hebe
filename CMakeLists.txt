cmake_minimum_required(VERSION 3.10)

# ===============================================
# Project setup

# Set the project name and version
project(Hebe VERSION 0.0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ===============================================
# Clangd linter config

# Save how files are compiled for clangd linter
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================================
# Spdlog library

# Load Spdlog library for logging
include(FetchContent)

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# ===============================================
# Bison and Flex

# Specify the location of the Bison executable when using Homebrew on macOS.
# When the variable is already set (e.g. inside Docker) we keep the existing value.
if(NOT DEFINED BISON_EXECUTABLE AND EXISTS "/usr/bin/bison")
    set(BISON_EXECUTABLE "/usr/bin/bison")
endif()

# Now, find Bison using the specified executable
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# ===============================================
# LLVM

# LLVM flags
execute_process(COMMAND llvm-config --cxxflags
    OUTPUT_VARIABLE LLVM_CXXFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --libs core
    OUTPUT_VARIABLE LLVM_LIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --system-libs
    OUTPUT_VARIABLE LLVM_SYSLIBS
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# Convert strings to CMake lists
string(REPLACE "\n" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
string(REPLACE "\n" " " LLVM_LDFLAGS "${LLVM_LDFLAGS}")
string(REPLACE "\n" " " LLVM_LIBS "${LLVM_LIBS}")
string(REPLACE "\n" " " LLVM_SYSLIBS "${LLVM_SYSLIBS}")

separate_arguments(LLVM_CXXFLAGS NATIVE_COMMAND "${LLVM_CXXFLAGS}")
separate_arguments(LLVM_LDFLAGS  NATIVE_COMMAND "${LLVM_LDFLAGS}")
separate_arguments(LLVM_LIBS     NATIVE_COMMAND "${LLVM_LIBS}")
separate_arguments(LLVM_SYSLIBS  NATIVE_COMMAND "${LLVM_SYSLIBS}")

# Remove LLVM's -std=c++14
list(FILTER LLVM_CXXFLAGS EXCLUDE REGEX "^-std=")

# Create the directory for generated files if it does not exist
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/generated)

# Generate parser.cpp and parser.hpp from grammar.y using Bison
BISON_TARGET(
    parser
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/bison/grammar.y
    ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.cpp
    DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/generated/parser.hpp
)

# Generate scanner.cpp from scanner.l using Flex
FLEX_TARGET(
    lexer
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/flex/scanner.l
    ${CMAKE_CURRENT_BINARY_DIR}/generated/scanner.cpp
)

# Ensure Bison runs before Flex because Flex imports a Bison header
ADD_FLEX_BISON_DEPENDENCY(lexer parser)

# Explicitly reference generated files from Bison and Flex
set(GENERATED_SOURCES
    ${BISON_parser_OUTPUTS}
    ${FLEX_lexer_OUTPUTS}
)

# Export compile_commands.json
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ===============================================
# Compilation

# Add all .cpp files in src to list SRC_FILES
file(GLOB_RECURSE SRC_FILES "src/*.cpp")

# Create executable main from all files in list SRC_FILES
add_executable(main ${SRC_FILES} ${GENERATED_SOURCES})

# Apply LLVM flags correctly (ensures libs appear AFTER objects)
target_compile_options(main PRIVATE ${LLVM_CXXFLAGS})
target_compile_options(main PRIVATE -fexceptions)

target_link_options(main PRIVATE ${LLVM_LDFLAGS})
target_link_libraries(main PRIVATE ${LLVM_LIBS} ${LLVM_SYSLIBS} spdlog::spdlog)

# Specify include directories for this target
target_include_directories(main PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/bison
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/flex
    ${CMAKE_CURRENT_BINARY_DIR}/generated
)

# -----------------------------------------------------------
# Custom target: clean_cache
# This target cleans cache and build files.
add_custom_target(clean_cache
  COMMAND ${CMAKE_COMMAND} -E rm -f  ${CMAKE_BINARY_DIR}/CMakeCache.txt
  COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/CMakeFiles
  COMMAND ${CMAKE_COMMAND} -E rm -f  ${CMAKE_BINARY_DIR}/build.ninja
  COMMAND ${CMAKE_COMMAND} -E rm -f  ${CMAKE_BINARY_DIR}/rules.ninja
  COMMENT "Delete cache/build files"
)

# -----------------------------------------------------------
# Custom target: clean_build_directory
# This target removes the build directory.
add_custom_target(clean_build_directory
  COMMAND ${CMAKE_COMMAND} -E rm -rf  ${CMAKE_BINARY_DIR}/build
  COMMENT "Delete build directory"
)

# -----------------------------------------------------------
# Custom target: run
# This target depends on the main executable and runs it.
add_custom_target(run
    COMMAND $<TARGET_FILE:main>
    DEPENDS main
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Running the main executable"
)

# -----------------------------------------------------------
# Custom target: run_generated
# This target runs an additional command (e.g. executing LLVM IR with lli).
find_program(LLI_EXECUTABLE lli REQUIRED)

add_custom_target(run_generated
    COMMAND $<TARGET_FILE:main>
    COMMAND ${LLI_EXECUTABLE} output_code.ll
    DEPENDS main
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Run compiler then execute output_code.ll with lli"
)
