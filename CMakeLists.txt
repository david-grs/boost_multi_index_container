cmake_minimum_required(VERSION 2.8)

add_definitions(" -D_GLIBCXX_USE_CXX11_ABI=1 ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -Wall -Wextra")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")

include_directories(.)
add_executable(main main.cc)

target_link_libraries(main papi)
