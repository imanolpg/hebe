cmake_minimum_required(VERSION 3.22)
project(hebe)

set(CMAKE_CXX_STANDARD 14)

# flags
set(CMAKE_CXX_FLAGS "-I/opt/homebrew/Cellar/llvm/13.0.1_1/include -std=c++14 -stdlib=libc++ -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -L/opt/homebrew/Cellar/llvm/13.0.1_1/lib -Wl,-search_paths_first -Wl,-headerpad_max_install_names -lLLVM-13")

add_executable(hebe main.cpp src/Lexer.cc src/AbstractSyntaxTree.cc src/Parser.cc src/CodeGeneration.cc)
