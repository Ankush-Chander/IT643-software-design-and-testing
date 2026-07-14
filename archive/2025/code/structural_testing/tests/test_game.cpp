#include "game.h"
#include <gtest/gtest.h>

TEST(GameTest, AddNumbers) {
    EXPECT_EQ(add(2, 3), 5);
    EXPECT_EQ(add(-1, 1), 0);
}

TEST(GameTest, CheckEven) {
    EXPECT_TRUE(is_even(4));
    EXPECT_FALSE(is_even(7));
}
