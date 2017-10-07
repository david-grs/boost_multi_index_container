cmake_minimum_required(VERSION 2.8)

project(boost_mic)

find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_compile_options(-std=c++14 -g)
#add_compile_options(-std=c++14 -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")


add_executable(stock stock.cc)
add_executable(session session.cc)
add_executable(employee_counter employee_counter.cc)
add_executable(memory memory.cc)
add_executable(integers integers.cc)

