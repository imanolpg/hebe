cmake_minimum_required(VERSION 3.10)

# ===============================================
# Project setup

project(Hebe VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(NOT CMAKE_CONFIGURATION_TYPES)
  set(allowed_build_types Debug Release)
  if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
  endif()
  list(FIND allowed_build_types "${CMAKE_BUILD_TYPE}" _idx)
  if(_idx EQUAL -1)
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE='${CMAKE_BUILD_TYPE}'. Use Debug or Release.")
  endif()
endif()

include(FetchContent)

# ===============================================
# FetchContent cache — keyed by platform/arch derived from the preset binary dir

string(REGEX MATCH "build/([^/]+)" _ ${CMAKE_BINARY_DIR})
set(PRESET_NAME ${CMAKE_MATCH_1})

if(PRESET_NAME MATCHES "^(linux|macos)-(arm64|x64)")
  set(PLATFORM_ARCH "${CMAKE_MATCH_1}-${CMAKE_MATCH_2}")
else()
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM "linux")
  elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM "macos")
  else()
    set(PLATFORM "unknown")
  endif()
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64|ARM64")
    set(ARCH "arm64")
  elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(ARCH "x64")
  else()
    set(ARCH "${CMAKE_SYSTEM_PROCESSOR}")
  endif()
  set(PLATFORM_ARCH "${PLATFORM}-${ARCH}")
endif()

set(FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.cache/fetchcontent/${PLATFORM_ARCH}"
    CACHE PATH "FetchContent cache directory")
message(STATUS "FetchContent cache: ${FETCHCONTENT_BASE_DIR}")

# ===============================================
# Spdlog

FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG v1.14.1
)
FetchContent_MakeAvailable(spdlog)

# ===============================================
# GoogleTest

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
FetchContent_MakeAvailable(googletest)

# ===============================================
# Bison and Flex

# Accept -DBISON_EXECUTABLE=... / -DFLEX_EXECUTABLE=... from the command line
# If not provided, keep whatever is already in cache. If still empty, set defaults
set(BISON_EXECUTABLE "" CACHE FILEPATH "Path to bison executable")
set(FLEX_EXECUTABLE  "" CACHE FILEPATH "Path to flex executable")

if(NOT BISON_EXECUTABLE AND EXISTS "/usr/bin/bison")
  set(BISON_EXECUTABLE "/usr/bin/bison" CACHE FILEPATH "Path to bison executable" FORCE)
endif()
if(NOT FLEX_EXECUTABLE AND EXISTS "/usr/bin/flex")
  set(FLEX_EXECUTABLE "/usr/bin/flex" CACHE FILEPATH "Path to flex executable" FORCE)
endif()

# Throw an error if BISON_EXECUTABLE does not exist
if(NOT BISON_EXECUTABLE)
  message(FATAL_ERROR
    "bison not found.\n"
    "Pass -DBISON_EXECUTABLE=/path/to/bison or install bison >= 3.0.0")
endif()

# Throw an error if FLEX_EXECUTABLE does not exist
if(NOT FLEX_EXECUTABLE)
  message(FATAL_ERROR
    "flex not found.\n"
    "Pass -DFLEX_EXECUTABLE=/path/to/flex or install flex.")
endif()

# Now, find Bison using the specified executable
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# ===============================================
# LLVM

# Accept: -DLLVM_CONFIG_EXECUTABLE=/path/to/llvm-config
set(LLVM_CONFIG_EXECUTABLE "" CACHE FILEPATH "Path to llvm-config executable")

if(NOT LLVM_CONFIG_EXECUTABLE AND NOT LLVM_CMAKE_DIR)
  if(EXISTS "/usr/bin/llvm-config")
    set(LLVM_CONFIG_EXECUTABLE "/usr/bin/llvm-config" CACHE FILEPATH "" FORCE)
  elseif(EXISTS "/usr/bin/llvm-config-21")
    set(LLVM_CONFIG_EXECUTABLE "/usr/bin/llvm-config-21" CACHE FILEPATH "" FORCE)
  endif()
endif()

if(LLVM_CONFIG_EXECUTABLE)
  # Mac/Linux path: use llvm-config
  message(STATUS "Using llvm-config: ${LLVM_CONFIG_EXECUTABLE}")
  execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --cxxflags
    OUTPUT_VARIABLE LLVM_CXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
    OUTPUT_VARIABLE LLVM_LDFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs core
    OUTPUT_VARIABLE LLVM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
    OUTPUT_VARIABLE LLVM_SYSLIBS OUTPUT_STRIP_TRAILING_WHITESPACE)
  execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Convert strings to CMake lists
  string(REPLACE "\n" " " LLVM_CXXFLAGS "${LLVM_CXXFLAGS}")
  string(REPLACE "\n" " " LLVM_LDFLAGS  "${LLVM_LDFLAGS}")
  string(REPLACE "\n" " " LLVM_LIBS     "${LLVM_LIBS}")
  string(REPLACE "\n" " " LLVM_SYSLIBS  "${LLVM_SYSLIBS}")

  separate_arguments(LLVM_CXXFLAGS NATIVE_COMMAND "${LLVM_CXXFLAGS}")
  separate_arguments(LLVM_LDFLAGS  NATIVE_COMMAND "${LLVM_LDFLAGS}")
  separate_arguments(LLVM_LIBS     NATIVE_COMMAND "${LLVM_LIBS}")
  separate_arguments(LLVM_SYSLIBS  NATIVE_COMMAND "${LLVM_SYSLIBS}")

  list(FILTER LLVM_CXXFLAGS EXCLUDE REGEX "^-std=")
  list(FILTER LLVM_CXXFLAGS EXCLUDE REGEX "^-fno-exceptions")
else()
  message(FATAL_ERROR
    "LLVM not found. Pass -DLLVM_CONFIG_EXECUTABLE=/path/to/llvm-config "
    "or -DLLVM_CMAKE_DIR=/path/to/llvm/lib/cmake/llvm")
endif()

# ===============================================
# Bison/Flex codegen

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

# ===============================================
# hebe_core — the library everything links against

file(GLOB_RECURSE SRC_FILES "src/*.cpp")

add_library(hebe_core STATIC ${SRC_FILES} ${GENERATED_SOURCES})

# PUBLIC so that main and hebe_tests inherit include paths and definitions
target_include_directories(hebe_core PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parser/bison
  ${CMAKE_CURRENT_SOURCE_DIR}/src/lexer/flex
  ${CMAKE_CURRENT_BINARY_DIR}/generated
)

target_include_directories(hebe_core PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_options(hebe_core PRIVATE ${LLVM_CXXFLAGS})
target_link_options(hebe_core PRIVATE ${LLVM_LDFLAGS})
target_link_libraries(hebe_core PUBLIC ${LLVM_LIBS} ${LLVM_SYSLIBS} spdlog::spdlog)

target_compile_definitions(hebe_core PUBLIC
  $<$<CONFIG:Debug>:HEBE_DEBUG>
  $<$<CONFIG:Release>:HEBE_RELEASE>
)

if(LLVM_DEFINITIONS)
  target_compile_definitions(hebe_core PUBLIC ${LLVM_DEFINITIONS})
endif()

# ===============================================
# main — thin executable that links hebe_core

add_executable(main src/main.cpp)
target_include_directories(main PRIVATE ${LLVM_INCLUDE_DIRS})
target_compile_options(main PRIVATE ${LLVM_CXXFLAGS})
target_link_options(main PRIVATE ${LLVM_LDFLAGS})
target_link_libraries(main PRIVATE hebe_core)

# ===============================================
# Release options

option(HEBE_ENABLE_LTO "Enable LTO/IPO in Release builds" ON)
option(HEBE_ENABLE_STRIP "Strip symbols in Release builds" ON)

set_property(TARGET hebe_core PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ${HEBE_ENABLE_LTO})
set_property(TARGET main      PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE ${HEBE_ENABLE_LTO})

if(HEBE_ENABLE_STRIP)
  target_link_options(main PRIVATE $<$<CONFIG:Release>:-s>)
endif()

# ===============================================
# Tests

enable_testing()

option(HEBE_ENABLE_TESTS "Build tests" OFF)

if(HEBE_ENABLE_TESTS)
  file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
  if(TEST_SOURCES)
    add_executable(hebe_tests ${TEST_SOURCES})
    target_link_libraries(hebe_tests PRIVATE hebe_core GTest::gtest_main)
    include(GoogleTest)
    gtest_discover_tests(hebe_tests)
  else()
    message(WARNING "No test sources found in tests/")
  endif()
endif()