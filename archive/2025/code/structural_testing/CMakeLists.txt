cmake_minimum_required(VERSION 3.10)
project(SnakeCoverageExample)

set(CMAKE_CXX_STANDARD 17)
# Compile with GCC using the -fprofile-arcs and -ftest-coverage flags (these enable gcov instrumentation)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage -O0 -g")

# GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS} src)

add_executable(runTests
    tests/test_game.cpp
    src/game.cpp
)
target_link_libraries(runTests ${GTEST_LIBRARIES} gtest_main pthread gcov)

