// Characterization tests for Game::update() — written for the course, not the student's.
#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>

#define class struct       // Game's members become public...
#define private public     // ...and so do Snake's and Food's
#define main snake_main    // keep the game's main() out of the way
#include "../snake.cpp"
#undef main
#undef private
#undef class

// A game with the snake at its start position (head (10,6), moving right)
// and the food wherever the test puts it.
static Game gameWithFoodAt(int x, int y) {
    Game g;
    g.snake.reset();
    g.food.pos = {x, y};
    return g;
}

TEST(Update, MovesOneCellPerTick) {
    Game g = gameWithFoodAt(0, 0);
    g.update();
    EXPECT_EQ(g.snake.head().x, 11);
    EXPECT_EQ(g.snake.head().y, 6);
}

TEST(Update, EatingFoodOneCellAhead) {
    Game g = gameWithFoodAt(11, 6);
    g.update();
    EXPECT_EQ(g.snake.head().x, 12);
    EXPECT_EQ(g.snake.getBody().size(), 4u);
    EXPECT_EQ(g.score, 1);
}

TEST(Update, EatingInLastColumn) {
    Game g = gameWithFoodAt(19, 6);
    g.snake.body = {{18, 6}, {17, 6}, {16, 6}};
    g.update();
    EXPECT_EQ(g.snake.head().x, 20);
    EXPECT_FALSE(g.gameOver);
}

TEST(Update, EatingSpeedsUpByFivePercent) {
    Game g = gameWithFoodAt(11, 6);
    g.update();
    EXPECT_DOUBLE_EQ(g.speedFactor, 0.95);
}

TEST(Update, HittingTheWallEndsTheGame) {
    Game g = gameWithFoodAt(0, 0);
    g.snake.body = {{19, 6}, {18, 6}, {17, 6}};
    g.update();
    EXPECT_TRUE(g.gameOver);
}
