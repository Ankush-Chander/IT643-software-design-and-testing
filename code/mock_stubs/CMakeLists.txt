cmake_minimum_required(VERSION 3.14)
project(WarehouseOrderTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find GoogleTest
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(order_test
    tests/order_test.cpp
)

target_include_directories(order_test PRIVATE lib)
target_link_libraries(order_test ${GTEST_LIBRARIES} pthread)

enable_testing()
add_test(NAME order_test COMMAND order_test)