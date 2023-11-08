cmake_minimum_required(VERSION 3.23)
project(cpp-examples)

find_package(kumi REQUIRED)
find_package(blaze REQUIRED)
link_libraries(kumi::kumi)
link_libraries(blaze::blaze)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# add_compile_options(-fconstexpr-steps=2147483647 -fbracket-depth=2147483647 -fconstexpr-depth=128)

add_executable(blaze-example blaze-example.cpp)
add_library(basic-constexpr basic-constexpr.cpp)
add_library(basic-tmp basic-tmp.cpp)
add_library(expression-templates expression-templates.cpp)
add_library(codegen-examples codegen-examples.cpp)
