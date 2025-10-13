# build tests

## for real objects
g++ -std=c++17 tests/order_test_real_objects.cpp -I/usr/include -lgmock -lgtest -pthread -o order_test_real_objects

## for manual test
g++ -std=c++17 tests/order_test_manual.cpp -I/usr/include -lgmock -lgtest -pthread -o order_test_manual

## for automatic test

g++ -std=c++17 tests/order_test.cpp -I/usr/include -lgmock -lgtest -pthread -o order_test