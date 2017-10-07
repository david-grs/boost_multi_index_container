cmake_minimum_required(VERSION 2.8)

project(boost_mic)

add_definitions(" -D_GLIBCXX_USE_CXX11_ABI=1 ")

find_package(Boost REQUIRED COMPONENTS)
include_directories(${Boost_INCLUDE_DIRS})

add_compile_options(-std=c++14 -g -Wall -Werror -Wextra -Wpedantic -Wconversion -Wswitch-default -Wswitch-enum -Wunreachable-code -Wwrite-strings -Wcast-align -Wshadow -Wundef)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	add_compile_options(-Weverything -Wno-c++98-compat -Wno-global-constructors)
endif()


add_executable(stock stock.cc)
add_executable(session session.cc)
add_executable(employee_counter employee_counter.cc)
add_executable(memory memory.cc)
add_executable(integers integers.cc)

