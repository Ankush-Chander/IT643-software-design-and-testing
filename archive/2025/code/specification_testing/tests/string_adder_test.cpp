/*
g++ -std=c++17 ../tests/string_adder_test.cpp -lgtest -lgtest_main -pthread -o ../tests/test_string_adder
*/

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../src/string_adder.h"

using namespace testing;


TEST(StringAdderTest, NoCarry){
    Solution s = Solution();
    string a("123");
    string b = "456"; 
    string result = s.addStrings(a, b);

    EXPECT_EQ("579", result);
}


TEST(StringAdderTest, CarryAtBeginning){
    Solution s = Solution();
    string a("1119");
    string b = "1232"; 
    string result = s.addStrings(a, b); 

    EXPECT_EQ("2351", result);
}


TEST(StringAdderTest, CarryInTheMiddle){
    Solution s = Solution();
    string a("183");
    string b = "456"; 
    string result = s.addStrings(a, b);

    EXPECT_EQ("639", result);
}


TEST(StringAdderTest, CarryAtEnd){
    Solution s = Solution();
    string a("123");
    string b = "999"; 
    string result = s.addStrings(a, b);

    EXPECT_EQ("1122", result);
}


TEST(StringAdderTest, MultipleCarries){
    Solution s = Solution();
    string a("999");
    string b = "999"; 
    string result = s.addStrings(a, b);

    EXPECT_EQ("1998", result);
}

