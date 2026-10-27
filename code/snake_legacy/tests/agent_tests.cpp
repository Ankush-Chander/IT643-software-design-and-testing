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

static Game gameWithFoodAt(int x, int y) { Game g; g.snake.reset(); g.food.pos = {x, y}; return g; }

TEST(Agent, MovesOneCellPerTick)            { Game g = gameWithFoodAt(0, 0); g.update(); EXPECT_EQ(g.snake.head().x, 11); }
TEST(Agent, EatingGrowsSnakeByOne)          { Game g = gameWithFoodAt(11, 6); g.update(); EXPECT_EQ(g.snake.getBody().size(), 4u); }
TEST(Agent, EatingMovesHeadTwoCells)        { Game g = gameWithFoodAt(11, 6); g.update(); EXPECT_EQ(g.snake.head().x, 12); }
TEST(Agent, EatingInLastColumnKeepsPlaying) { Game g = gameWithFoodAt(19, 6); g.snake.body = {{18,6},{17,6},{16,6}}; g.update();
                                              EXPECT_EQ(g.snake.head().x, 20); EXPECT_FALSE(g.gameOver); }
TEST(Agent, SpeedStopsAt0_292) {
    Game g = gameWithFoodAt(0, 0);
    for (int i = 0; i < 100; ++i) {            // eat 100 times
        g.snake.reset(); g.food.pos = {11, 6}; g.gameOver = false; g.update();
    }
    EXPECT_NEAR(g.speedFactor, 0.292, 0.001);
}
