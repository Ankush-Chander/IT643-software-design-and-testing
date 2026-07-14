#include <gtest/gtest.h>

// Function to be tested
int Add(int a, int b) {
    return a + b;
}

// Test case for the Add function
TEST(AddTest, PositiveNumbers) {
    EXPECT_EQ(Add(2, 3), 5);
    EXPECT_EQ(Add(10, 20), 30);
}

TEST (AddTest, NegativeNumbers) {
    EXPECT_EQ(Add(-2, -3), -5);
    EXPECT_EQ(Add(-10, -20), -30);
}


TEST (AddTest, Zero) {
    // FATAL FAIL
    // ASSERT_EQ(Add(0, 0), 1);
    
    // NON FATAL FAIL
    EXPECT_EQ(Add(0, 0), 1) << "The values were expected to be equal for this specific input."; 
    std::cout << "hello";
    EXPECT_EQ(Add(0, 10), 10);
    EXPECT_EQ(Add(10, 0), 10);
}


// g++ -o simple_test simple_test.cpp -lgtest -lgtest_main -pthread; ./simple_test
