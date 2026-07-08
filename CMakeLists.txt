cmake_minimum_required(VERSION 3.15)
project(math)

set(CMAKE_CXX_STANDARD 23) 
set(CMAKE_CXX_STANDARD_REQUIRED ON) 



add_executable(math
    src/main.cpp
    src/tokenizer.cpp
    src/symbols.cpp
    src/value.cpp
)
target_include_directories(math
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(math 
    PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -fsanitize=address,undefined
)
target_link_options(math PRIVATE -fsanitize=address,undefined)