/*
g++ -std=c++17 ../tests/string_between_test.cpp -lgtest -lgtest_main -pthread -o ../tests/test_substrings
*/

#include "gtest/gtest.h"
#include <vector>
#include <string>
#include <algorithm>
#include "../src/substrings_between.h"

using namespace testing;

// Helper function to compare two string vectors for equality
AssertionResult VectorsEqual(const std::vector<std::string>& a,
                                        const std::vector<std::string>& b) {
    if (a == b) return AssertionSuccess();

    std::string msg = "Expected: [";
    for (const auto& s : b) msg += s + " ";
    msg += "] but got [";
    for (const auto& s : a) msg += s + " ";
    msg += "]";
    return AssertionFailure() << msg;
}

/*
Tests related to the string being null or empty.
*/
TEST(StringUtilsTest, StrIsNullOrEmpty) {
    
    EXPECT_TRUE(substringsBetween("", "a", "b").empty());
}

/*
Tests related to open or close being null or empty.
*/
TEST(StringUtilsTest, OpenIsNullOrEmpty) {
    EXPECT_TRUE(substringsBetween("abc", "", "b").empty());
}

TEST(StringUtilsTest, CloseIsNullOrEmpty) {
    EXPECT_TRUE(substringsBetween("abc", "a", "").empty());
}

TEST(StringUtilsTest, BothOpenAndCloseIsNullOrEmpty) {
    EXPECT_TRUE(substringsBetween("abc", "", "").empty());
}

/*
Tests related to string and open and close tags with length 1.
*/
TEST(StringUtilsTest, StrOfLength1) {
    EXPECT_TRUE(substringsBetween("a", "a", "b").empty());
    EXPECT_TRUE(substringsBetween("a", "b", "a").empty());
    EXPECT_TRUE(substringsBetween("a", "b", "b").empty());
    EXPECT_TRUE(substringsBetween("a", "a", "a").empty());
}

TEST(StringUtilsTest, OpenAndCloseOfLength1) {
    EXPECT_TRUE(substringsBetween("abc", "x", "y").empty());
    EXPECT_TRUE(substringsBetween("abc", "a", "y").empty());
    EXPECT_TRUE(substringsBetween("abc", "x", "c").empty());
    EXPECT_TRUE(VectorsEqual(substringsBetween("abc", "a", "c"), {"b"}));
    EXPECT_TRUE(VectorsEqual(substringsBetween("abcabc", "a", "c"), {"b", "b"}));
}

/*
Tests for the open and close tags of varying sizes.
*/
TEST(StringUtilsTest, OpenAndCloseTagsOfDifferentSizes) {
    EXPECT_TRUE(substringsBetween("aabcc", "xx", "yy").empty());
    EXPECT_TRUE(substringsBetween("aabcc", "aa", "yy").empty());
    EXPECT_TRUE(substringsBetween("aabcc", "xx", "cc").empty());
    EXPECT_TRUE(VectorsEqual(substringsBetween("aabbcc", "aa", "cc"), {"bb"}));
    EXPECT_TRUE(VectorsEqual(substringsBetween("aabbccaaeecc", "aa", "cc"), {"bb", "ee"}));
}


/*
Test for when there is no substring between the open and close tags.
*/
TEST(StringUtilsTest, NoSubstringBetweenOpenAndCloseTags) {
    EXPECT_TRUE(VectorsEqual(substringsBetween("aabb", "aa", "bb"), {""}));
}



TEST(StringUtilsTest, SameOpenAndCloseTags){
    EXPECT_TRUE(VectorsEqual(substringsBetween("abcdabefgab", "ab", "ab"), {"cd", "efg"}));
}