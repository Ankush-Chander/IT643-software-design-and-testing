#include <gtest/gtest.h>
#define main snake_main
#include "../test.cpp"
#undef main

class SnakeTest : public ::testing::Test {
protected:
    void SetUp() override { stub::reset(); lup = 0; sc = 0; run = true; }
};

// ---------- eid ----------
TEST_F(SnakeTest, EidFindsElement) {
    deque<Vector2> d = {{1,2},{3,4}};
    EXPECT_TRUE(eid(Vector2{3,4}, d));
}
TEST_F(SnakeTest, EidMissingElement) {
    deque<Vector2> d = {{1,2},{3,4}};
    EXPECT_FALSE(eid(Vector2{5,5}, d));
}
TEST_F(SnakeTest, EidEmptyDeque) {
    EXPECT_FALSE(eid(Vector2{0,0}, {}));
}

// ---------- eventTrigger ----------
TEST_F(SnakeTest, EventTriggerFiresWhenIntervalElapsed) {
    stub::time = 0.5;
    EXPECT_TRUE(eventTrigger(0.3));
    EXPECT_DOUBLE_EQ(lup, 0.5);
}
TEST_F(SnakeTest, EventTriggerDoesNotFireBeforeInterval) {
    stub::time = 0.5; eventTrigger(0.3);
    stub::time = 0.6;
    EXPECT_FALSE(eventTrigger(0.3));
    EXPECT_DOUBLE_EQ(lup, 0.5);
}

// ---------- Food ----------
TEST_F(SnakeTest, FoodConstructorLoadsTextureAndPlacesOffSnake) {
    stub::randomValues = {0,0, 5,7};   // first (0,0) is on snake -> retry
    {
        Food f({{0,0},{1,0}});
        EXPECT_EQ(f.p.x, 5); EXPECT_EQ(f.p.y, 7);
        EXPECT_EQ(stub::textureLoads, 1);
    }
    EXPECT_EQ(stub::textureUnloads, 1);
}
TEST_F(SnakeTest, FoodGrpFirstTryWhenFree) {
    Food f({});                      // constructor consumes its own random values
    stub::randomValues = {3,4};      // script the values grp() will see
    Vector2 p = f.grp({{0,0}});
    EXPECT_EQ(p.x, 3); EXPECT_EQ(p.y, 4);
}
TEST_F(SnakeTest, FoodDrawCallsTexture) {
    Food f({});
    f.Draw();
    EXPECT_EQ(stub::textureDraws, 1);
}

// ---------- Snake ----------
TEST_F(SnakeTest, SnakeInitialState) {
    Snake s;
    ASSERT_EQ(s.body.size(), 2u);
    EXPECT_EQ(s.body[0].x, 1); EXPECT_EQ(s.direction.x, 1);
    EXPECT_FALSE(s.addb);
}
TEST_F(SnakeTest, SnakeUpdateMovesWithoutGrowing) {
    Snake s; s.update();
    ASSERT_EQ(s.body.size(), 2u);
    EXPECT_EQ(s.body[0].x, 2); EXPECT_EQ(s.body[1].x, 1);
}
TEST_F(SnakeTest, SnakeUpdateGrowsWhenAddb) {
    Snake s; s.addb = true; s.update();
    EXPECT_EQ(s.body.size(), 3u);
    EXPECT_FALSE(s.addb);
}
TEST_F(SnakeTest, SnakeDrawDrawsEachSegment) {
    Snake s; s.addb = true; s.update(); s.Draw();
    EXPECT_EQ(stub::rectDraws, 3);
}
TEST_F(SnakeTest, SnakeResetRestoresDefaultsAndStops) {
    Snake s; s.direction = {0,1}; s.addb = true; s.update(); s.update();
    s.reset();
    EXPECT_EQ(s.body.size(), 2u);
    EXPECT_EQ(s.direction.x, 1); EXPECT_EQ(s.direction.y, 0);
    EXPECT_FALSE(run);
}

// ---------- go ----------
TEST_F(SnakeTest, GoResetsScoreSnakeAndFood) {
    Snake s; Food f(s.body); sc = 9;
    stub::randomValues = {8,8};
    go(s, f);
    EXPECT_EQ(sc, 0);
    EXPECT_EQ(f.p.x, 8);
    EXPECT_FALSE(run);
}

// ---------- main loop ----------
TEST_F(SnakeTest, MainLoopRunsFramesAndDraws) {
    stub::framesUntilClose = 3;
    stub::randomValues = {10,10};
    EXPECT_EQ(snake_main(), 0);
    EXPECT_EQ(stub::textureDraws, 3);
    EXPECT_EQ(stub::textDraws, 6);
}
TEST_F(SnakeTest, MainLoopKeyPresses) {
    // 4 frames, each with a different key; direction rules exercised
    stub::framesUntilClose = 4;
    stub::randomValues = {10,10};
    stub::keys = {{KEY_UP,true},{KEY_DOWN,true},{KEY_LEFT,true},{KEY_RIGHT,true}};
    EXPECT_EQ(snake_main(), 0);
}
TEST_F(SnakeTest, MainLoopEatsFoodAndScores) {
    // food placed at (2,0); snake head at (1,0) moving right; time elapsed -> update -> eat
    stub::framesUntilClose = 1;
    stub::randomValues = {2,0, 10,10};
    stub::time = 1.0;
    snake_main();
    EXPECT_EQ(sc, 1);
    EXPECT_EQ(stub::soundPlays, 1);
}
TEST_F(SnakeTest, MainLoopHitsRightWallAndResets) {
    // Head starts at x==1 moving right. With time advancing 1s per frame every
    // frame triggers an update, so the head reaches x==cellcount on frame 24.
    stub::framesUntilClose = 30;
    stub::randomValues = {10,10, 12,12};   // initial food, then food after go()
    stub::time = 1.0; stub::timeStep = 1.0;
    sc = 5;
    EXPECT_EQ(snake_main(), 0);
    EXPECT_GE(stub::soundPlays, 1);        // wall sound
    EXPECT_EQ(sc, 0);                      // go() reset the score
    EXPECT_FALSE(run);                     // go() -> Snake::reset() stops the game
}
TEST_F(SnakeTest, MainLoopHitsTopWall) {
    // Frame 1: update moves right to x==2, then KEY_UP turns the snake up.
    // Frame 2: update moves to y==-1 -> top wall -> go() + wall sound.
    stub::framesUntilClose = 2;
    stub::randomValues = {10,10, 12,12};
    stub::keys = {{KEY_UP,true}};
    stub::time = 1.0; stub::timeStep = 1.0;
    sc = 3;
    EXPECT_EQ(snake_main(), 0);
    EXPECT_GE(stub::soundPlays, 1);
    EXPECT_EQ(sc, 0);
}
