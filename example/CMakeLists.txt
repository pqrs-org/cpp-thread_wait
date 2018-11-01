cmake_minimum_required (VERSION 3.9)

set(CMAKE_CXX_STANDARD 14)

add_compile_options(-Wall)
add_compile_options(-Werror)
add_compile_options(-O2)

project (test)

include_directories(SYSTEM ${CMAKE_CURRENT_LIST_DIR}/../include)

add_executable(
  test
  main.cpp
)
