#include <gtest/gtest.h>

// Function to be tested
int Add(int a, int b) {
    return a + b;
}

// Define a struct to hold parameters
struct AddTestCase {
    int a;
    int b;
    int expected;
};

// Define the fixture for parameterized tests
class AddTest : public ::testing::TestWithParam<AddTestCase> {};

// Parameterized test
TEST_P(AddTest, HandlesVariousInputs) {
    auto param = GetParam();
    EXPECT_EQ(Add(param.a, param.b), param.expected);
}

// Instantiate test suite with multiple cases
INSTANTIATE_TEST_SUITE_P(
    AddTests,
    AddTest,
    ::testing::Values(
        AddTestCase{2, 3, 5},        // Positive numbers
        AddTestCase{10, 20, 30},
        AddTestCase{-2, -3, -5},     // Negative numbers
        AddTestCase{-10, -20, -30},
        AddTestCase{0, 0, 0},        // Zero cases
        AddTestCase{0, 10, 10},
        AddTestCase{10, 0, 10}
    )
);

// Compile with:
// g++ -o parameterized_test parameterized_test.cpp -lgtest -lgtest_main -pthread
