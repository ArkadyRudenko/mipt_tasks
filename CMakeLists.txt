cmake_minimum_required(VERSION 3.20)
project(mipt_tasks)

#SET(MYCOMPILE_FLAGS "-Wall -Wextra")

set(Sanitizers 1)

if(Sanitizers EQUAL 1)
    message("Compile with Sanitizers")
    set(CMAKE_C_COMPILER "clang++-9")
    set(CMAKE_CXX_COMPILER "/usr/bin/clang++-9")

    set(MYCOMPILE_FLAGS "-std=c++17 -pedantic  -fsanitize=address -g -fno-omit-frame-pointer -fno-optimize-sibling-calls")
    set(CMAKE_CXX_FLAGS ${MYCOMPILE_FLAGS})

    add_definitions("-D GLIBCXX_DEBUG -D GLIBCXX_DEBUG_PEDANTIC")
endif()


include_directories(includes/tests)
include_directories(includes/tests/list_and_allocators)
include_directories(includes/tests/deque_test)
include_directories(includes/tests/string_test)
include_directories(includes/string)
include_directories(includes/deque)
include_directories(includes/list_and_allocators)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})

set_target_properties(
        ${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
)