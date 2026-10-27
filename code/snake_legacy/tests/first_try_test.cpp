#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#define class struct
#define private public
#define main snake_main
#include "../snake.cpp"
#undef main
#undef private
#undef class

TEST(Update, EatingFoodOneCellAhead) {
    Game g;
    g.snake.reset();
    g.food.pos = {11, 6};
    g.update();
    EXPECT_EQ(g.snake.head().x, 0);    // no idea yet: let the failure tell us
}
