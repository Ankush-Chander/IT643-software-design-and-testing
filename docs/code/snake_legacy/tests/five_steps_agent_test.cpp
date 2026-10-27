// Test harness for Game::update() in ../snake.cpp, without editing snake.cpp.
//
// How it gets access:
//   - Every standard header snake.cpp uses is included FIRST, so its include
//     guards are already set and the macros below never reach library code.
//   - `class` -> `struct` and `private` -> `public` make every member of
//     Snake, Food and Game public (Game and Food rely on class-default private
//     access, so `private` alone is not enough).
//   - `main` -> `snake_main` keeps snake.cpp's main() from clashing with
//     gtest_main.
// Redefining keywords is formally undefined behaviour, but GCC and Clang
// accept it; it is confined to this one translation unit.
//
// Build (from this directory):
//   g++ -std=c++17 update_test.cpp -lgtest -lgtest_main -pthread -o update_test

#include <iostream>
#include <vector>
#include <string>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <cstdlib>
#include <ctime>

#include <gtest/gtest.h>

#define class struct
#define private public
#define main snake_main
#include "../snake.cpp"
#undef main
#undef private
#undef class

// Characterization tests: they record what Game::update() does today,
// including behaviour that looks like a bug. Do not "fix" an expectation
// here without deciding to change the game's behaviour.

namespace {

// A started game with the food parked at (0,0), away from the snake's path.
Game freshGame() {
    Game g;
    g.restart();
    g.food.pos = {0, 0};
    return g;
}

void placeSnake(Game &g, std::vector<Position> body, Snake::Dir dir) {
    g.snake.body = std::move(body);
    g.snake.dir = dir;
}

void expectBody(const Game &g, const std::vector<Position> &expected) {
    const auto &body = g.snake.getBody();
    ASSERT_EQ(body.size(), expected.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        EXPECT_EQ(body[i].x, expected[i].x) << "segment " << i;
        EXPECT_EQ(body[i].y, expected[i].y) << "segment " << i;
    }
}

}  // namespace

// Start state, one tick: the snake moves one cell right and nothing else changes.
TEST(GameUpdate, OneTickFromStartMovesHeadRight) {
    Game g;
    g.restart();
    g.food.pos = {0, 0};  // keep the random food out of the snake's path

    g.update();

    const auto &body = g.snake.getBody();
    ASSERT_EQ(body.size(), 3u);
    EXPECT_EQ(body[0].x, 11); EXPECT_EQ(body[0].y, 6);
    EXPECT_EQ(body[1].x, 10); EXPECT_EQ(body[1].y, 6);
    EXPECT_EQ(body[2].x, 9);  EXPECT_EQ(body[2].y, 6);
    EXPECT_EQ(g.score, 0);
    EXPECT_EQ(g.highScore, 0);
    EXPECT_FALSE(g.gameOver);
    EXPECT_DOUBLE_EQ(g.speedFactor, 1.0);
}

// ---- Movement --------------------------------------------------------------

TEST(GameUpdate, MovesInCurrentDirection) {
    Game g = freshGame();
    g.snake.dir = Snake::UP;
    g.update();
    expectBody(g, {{10, 5}, {10, 6}, {9, 6}});
    EXPECT_FALSE(g.gameOver);
}

TEST(GameUpdate, MovingIntoTheCellTheTailIsLeavingIsNotACollision) {
    Game g = freshGame();
    // 2x2 loop; heading down moves the head onto the current tail cell.
    placeSnake(g, {{5, 5}, {6, 5}, {6, 6}, {5, 6}}, Snake::DOWN);
    g.update();
    expectBody(g, {{5, 6}, {5, 5}, {6, 5}, {6, 6}});
    EXPECT_FALSE(g.gameOver);
}

// ---- Walls -----------------------------------------------------------------

TEST(GameUpdate, RightWallEndsGameOnTenthTickFromStart) {
    Game g = freshGame();
    for (int i = 0; i < 9; ++i) {
        g.update();
        ASSERT_FALSE(g.gameOver) << "tick " << i + 1;
    }
    EXPECT_EQ(g.snake.head().x, 19);
    g.update();
    EXPECT_TRUE(g.gameOver);
}

TEST(GameUpdate, WallHitLeavesHeadOffTheBoard) {
    Game g = freshGame();
    placeSnake(g, {{19, 6}, {18, 6}, {17, 6}}, Snake::RIGHT);
    g.update();
    EXPECT_TRUE(g.gameOver);
    // The move is not undone: the head sits outside the board.
    expectBody(g, {{20, 6}, {19, 6}, {18, 6}});
    EXPECT_EQ(g.score, 0);
}

TEST(GameUpdate, TopWallEndsGame) {
    Game g = freshGame();
    placeSnake(g, {{4, 0}, {4, 1}, {4, 2}}, Snake::UP);
    g.update();
    EXPECT_TRUE(g.gameOver);
    EXPECT_EQ(g.snake.head().y, -1);
}

TEST(GameUpdate, LeftWallEndsGame) {
    Game g = freshGame();
    placeSnake(g, {{0, 3}, {1, 3}, {2, 3}}, Snake::LEFT);
    g.update();
    EXPECT_TRUE(g.gameOver);
    EXPECT_EQ(g.snake.head().x, -1);
}

TEST(GameUpdate, BottomWallEndsGame) {
    Game g = freshGame();
    placeSnake(g, {{4, 11}, {4, 10}, {4, 9}}, Snake::DOWN);
    g.update();
    EXPECT_TRUE(g.gameOver);
    EXPECT_EQ(g.snake.head().y, 12);
}

// ---- Self collision --------------------------------------------------------

TEST(GameUpdate, RunningIntoOwnBodyEndsGame) {
    Game g = freshGame();
    // Head (5,5) turns down into (5,6), which is a body segment, not the tail.
    placeSnake(g, {{5, 5}, {6, 5}, {6, 6}, {5, 6}, {4, 6}}, Snake::DOWN);
    g.update();
    EXPECT_TRUE(g.gameOver);
    expectBody(g, {{5, 6}, {5, 5}, {6, 5}, {6, 6}, {5, 6}});
    EXPECT_EQ(g.score, 0);
}

// ---- update() ignores gameOver ---------------------------------------------

TEST(GameUpdate, KeepsMovingWhenAlreadyGameOver) {
    Game g = freshGame();
    g.gameOver = true;
    g.update();
    expectBody(g, {{11, 6}, {10, 6}, {9, 6}});
    EXPECT_TRUE(g.gameOver);
}

// ---- Eating food -----------------------------------------------------------

TEST(GameUpdate, EatingFoodScoresGrowsAndMovesTwoCells) {
    Game g = freshGame();
    g.food.pos = {11, 6};  // directly in front of the head
    g.update();

    EXPECT_EQ(g.score, 1);
    EXPECT_EQ(g.highScore, 1);
    EXPECT_FALSE(g.gameOver);
    // Grows by one AND the head advances a second cell in the same tick.
    expectBody(g, {{12, 6}, {11, 6}, {10, 6}, {9, 6}});
    EXPECT_DOUBLE_EQ(g.speedFactor, 0.95);
}

TEST(GameUpdate, EatingFoodRespawnsItOnTheBoardOffTheSnake) {
    for (unsigned seed = 0; seed < 50; ++seed) {
        Game g = freshGame();
        srand(seed);
        g.food.pos = {11, 6};
        g.update();

        Position f = g.food.get();
        EXPECT_GE(f.x, 0);  EXPECT_LT(f.x, WIDTH);
        EXPECT_GE(f.y, 0);  EXPECT_LT(f.y, HEIGHT);
        EXPECT_FALSE(g.snake.occupies(f)) << "seed " << seed;
    }
}

TEST(GameUpdate, HighScoreOnlyRisesWhenScorePassesIt) {
    Game g = freshGame();
    g.highScore = 5;
    g.food.pos = {11, 6};
    g.update();
    EXPECT_EQ(g.score, 1);
    EXPECT_EQ(g.highScore, 5);
}

TEST(GameUpdate, SecondMoveAfterEatingIsNotWallChecked) {
    Game g = freshGame();
    placeSnake(g, {{18, 6}, {17, 6}, {16, 6}}, Snake::RIGHT);
    g.food.pos = {19, 6};  // last column
    g.update();

    EXPECT_EQ(g.score, 1);
    // The extra step puts the head at x = 20, off the board, yet the game
    // carries on until the next tick.
    expectBody(g, {{20, 6}, {19, 6}, {18, 6}, {17, 6}});
    EXPECT_FALSE(g.gameOver);

    g.update();
    EXPECT_TRUE(g.gameOver);
}

TEST(GameUpdate, SecondMoveAfterEatingIsNotSelfChecked) {
    Game g = freshGame();
    // Moving down: first step eats at (5,6); the second step lands on (5,7),
    // which is a body segment.
    placeSnake(g, {{5, 5}, {4, 5}, {4, 6}, {4, 7}, {5, 7}, {6, 7}}, Snake::DOWN);
    g.food.pos = {5, 6};
    g.update();

    EXPECT_EQ(g.score, 1);
    EXPECT_TRUE(g.snake.collidesWithSelf());
    EXPECT_FALSE(g.gameOver);
}

TEST(GameUpdate, SpeedFactorShrinksByFivePercentWhileAbovePointThree) {
    Game g = freshGame();
    g.speedFactor = 0.31;
    g.food.pos = {11, 6};
    g.update();
    // The check runs before the multiply, so it can dip below 0.3 once.
    EXPECT_DOUBLE_EQ(g.speedFactor, 0.31 * 0.95);
}

TEST(GameUpdate, SpeedFactorStopsChangingAtOrBelowPointThree) {
    Game g = freshGame();
    g.speedFactor = 0.3;
    g.food.pos = {11, 6};
    g.update();
    EXPECT_DOUBLE_EQ(g.speedFactor, 0.3);
}

TEST(GameUpdate, SpeedFactorBottomsOutAfterTwentyFourFoods) {
    Game g = freshGame();
    double expected = 1.0;
    for (int i = 0; i < 30; ++i) {
        placeSnake(g, {{10, 6}, {9, 6}, {8, 6}}, Snake::RIGHT);
        g.food.pos = {11, 6};
        g.update();
        if (i < 24) expected *= 0.95;
        EXPECT_DOUBLE_EQ(g.speedFactor, expected) << "food " << i + 1;
    }
    EXPECT_EQ(g.score, 30);
    EXPECT_NEAR(g.speedFactor, 0.2920, 0.0001);
}
