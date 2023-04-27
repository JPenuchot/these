cmake_minimum_required(VERSION 3.23)
project(cpp-examples)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-fconstexpr-steps=2147483647 -fbracket-depth=2147483647 -fconstexpr-depth=128)

add_library(tree-pass-by-generator tree-pass-by-generator.cpp)
