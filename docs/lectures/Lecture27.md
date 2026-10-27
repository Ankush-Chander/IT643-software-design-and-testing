# Legacy code: code without tests

## Quick recap
- Interfaces people can use: least astonishment, tell don't ask, progressive disclosure

---
### Agenda

- What makes code legacy
- Edit and pray vs cover and modify
- The legacy code change algorithm, with an agent in the loop
- Characterization tests: pin what the code does today

---
## The game

A snake game written by a student in this course: one file, 244 lines, no tests.
[`Heer117/Snake_Game` — `snake.cpp`](https://github.com/Heer117/Snake_Game/blob/e15f61a1e3830dc100a64098e7b2c03adb46c217/snake.cpp)

```cpp
void update() {                                       // called once per frame
    snake.move();
    Position h = snake.head();

    if (h.x < 0 || h.x >= WIDTH || h.y < 0 || h.y >= HEIGHT || snake.collidesWithSelf()) {
        gameOver = true;
        return;
    }

    if (h == food.get()) {
        score++;
        snake.move(true);                             // grow
        food.spawn(snake);
        if (score > highScore) highScore = score;
        if (speedFactor > 0.3) speedFactor *= 0.95;   // speed increases gradually
    }
}
```

An agent is about to add a pause key. **After it's done, how would you know nothing else
changed?** Would you (a) play the game, (b) read the diff, or (c) trust the agent?

??? hint "Answer"
    Each one checks something, and none of them is enough:

    | Check | What it misses |
    |---|---|
    | play it | anything you didn't happen to do in those two minutes |
    | read the diff | what the changed lines do to the code you didn't read |
    | trust the agent | it reports what it meant to do, not what it did |

    None of them **tells you** when behaviour changes. That is what a test does.

---
## What makes code legacy

> To me, legacy code is simply code without tests.
> — Michael Feathers, *Working Effectively with Legacy Code*

Not old code. Not someone else's code. **Code you can't change with confidence.**

| | |
|---|---|
| Snake games submitted for Lab 1 | 37 |
| ...with any test at all | **0** |

**Is code an agent wrote for you an hour ago legacy?**

??? hint "Answer"
    **Yes, if it has no tests.** Age doesn't matter. Generated code with no tests is legacy
    the moment it lands, and agents produce it faster than anyone can read it.

---
## Edit and pray vs cover and modify

| | Edit and pray | Cover and modify |
|---|---|---|
| Before the change | plan carefully, read the code | put tests around the area |
| After the change | run it, poke at it, hope | run the tests |
| Who notices a break | a user, later | the test, now |

> Unfortunately, Edit and Pray is pretty much the industry standard. — Feathers

**"The agent made the change, the game still runs, merge it." Which one is that?**

??? hint "Answer"
    **Edit and pray**, with the agent doing the editing. Being careful doesn't change that:
    without tests, care is the only safety net, and it doesn't scale to an agent's speed.

---
## The legacy code change algorithm

Feathers' five steps for changing legacy code:

1. Identify change points
2. Find test points
3. Break dependencies
4. Write tests
5. Make changes and refactor

**With an agent in the loop, which of these can you hand over?**

??? hint "Answer"
    | Step | The agent can | You must decide |
    |---|---|---|
    | 1. Change points | suggest where pause could go | where it **should** go, and what must not change |
    | 2. Test points | list what's observable | which behaviour matters enough to pin |
    | 3. Break dependencies | do it mechanically | whether the scar is acceptable |
    | 4. Write tests | write a dozen in a minute | **whether each pinned behaviour is right** |
    | 5. Change and refactor | make the change | whether the tests still mean what they did |

    The agent does the typing in every step. The right-hand column is the job.

---
#### Your turn: open the game

[Open the course repo in Codespaces](https://codespaces.new/Ankush-Chander/IT643-software-design-and-testing),
or clone it on Linux/macOS. The game is in `docs/code/snake_legacy/`.

Use the agent you have: Copilot's agent mode in the Chat panel, or `claude` in the terminal.

---

!!! example "🧑‍💻 Your turn — step 1: identify change points"

    Paste this to your agent:

    ```
    Read docs/code/snake_legacy/snake.cpp. I want to add pause: P pauses, P again
    resumes. Don't write any code. List the functions that would have to change,
    and why.
    ```

    **How many functions did your agent list? Are they the ones you would have picked?**

    ??? hint "Your decision"
        The agent lists candidates. You decide where pause **should** go, and say what
        must **not** change: `update()`, the rules of the game.

---

!!! example "🧑‍💻 Your turn — step 2: find test points"

    ```
    Which behaviour of Game::update() in docs/code/snake_legacy/snake.cpp could a
    test observe without a terminal? List the values. Don't write tests yet.
    ```

    **Which of those values would a player notice first if it changed?**

    ??? hint "Your decision"
        Pin what a player would notice: head position, length, score, `gameOver`.
        Speed matters less. The choice is yours, not the agent's.

---
## Characterization tests

A **characterization test** records what the code does **now**, not what it should do.

Its job is to tell you when behaviour changes. Whether that behaviour is correct is a
separate question.

The next three slides walk through tests written for this course, in
`docs/code/snake_legacy/tests/`. After that, your agent writes its own.

---
#### Getting `update()` into a test

From `tests/update_test.cpp`. `update()` is private, and `Game` owns everything. Break in without editing the file:

```cpp
#include <gtest/gtest.h>
// ...standard headers first...

#define class struct       // Game's members become public...
#define private public     // ...and so do Snake's and Food's
#define main snake_main    // keep the game's main() out of the way
#include "snake.cpp"
```

```cpp
static Game gameWithFoodAt(int x, int y) {      // head at (10,6), moving right
    Game g;
    g.snake.reset();
    g.food.pos = {x, y};
    return g;
}
```

Ugly, and deliberate. The student's file is untouched, so nothing changed before the tests
existed.

---
#### Writing one

From `tests/first_try_test.cpp`. You don't know what eating does yet. Write a test you **expect to fail**:

```cpp
TEST(Update, EatingFoodOneCellAhead) {
    Game g = gameWithFoodAt(11, 6);              // food right in front of the head
    g.update();
    EXPECT_EQ(g.snake.head().x, 0);              // no idea yet
}
```

**What does the failure tell you?**

??? hint "Answer"
    ```
    Failure
    Expected equality of these values:
      g.snake.head().x
        Which is: 12
      0
    ```

    The code says **12**. So the test says 12:

    ```cpp
    EXPECT_EQ(g.snake.head().x, 12);
    ```

    That's the whole method: let the code tell you what it does, then write it down.

---
#### The surprise

The head started at 10, the food was at 11, and the head ended at **12**.

```cpp
snake.move();          // step 1: onto the food
...
snake.move(true);      // step 2: grow, by moving again
```

**Eating moves the snake two cells, and the second step is never checked. Bug or feature?**

??? hint "Answer"
    The test can't tell you, and neither can the code. A player might never notice. Or a
    player might rely on the jump to dodge something.

    Two places it clearly goes wrong, both pinned by passing tests:

    | Setup | After one tick |
    |---|---|
    | food in the last column, x = 19 | head at **x = 20**, off the board, `gameOver = false` |
    | second step lands on the body | `collidesWithSelf() = true`, `gameOver = false` |

    **You decide, and write the decision down**: name the test after what it pins, and
    mark the ones you believe are bugs. Fix them later, on purpose, in a separate change.

---

!!! example "🧑‍💻 Your turn — step 4: write tests"

    ```
    Write GoogleTest characterization tests for Game::update() in
    docs/code/snake_legacy/snake.cpp. Record what it does now, not what it should
    do. Include eating food. Don't edit snake.cpp, and don't read the other files
    in tests/. Put the tests in docs/code/snake_legacy/tests/my_update_test.cpp,
    then build and run them.
    ```

    To build and run them yourself: `cd docs/code/snake_legacy/tests && make my_update_test && ./my_update_test`

    **Read the tests, not the code.** Find any test where eating moves the head two cells.
    Did your agent pin it as correct, flag it, or miss it?

    ??? hint "Your decision"
        Each pinned behaviour is correct, a bug, or a question for whoever owns the game.
        The agent can't tell which. Mark them.

        Then compare with `tests/five_steps_agent_test.cpp`: 18 tests another agent
        session wrote for the same prompt. Did yours find the same behaviours?

---
#### Let the agent write them, you judge them

An agent writes characterization tests in seconds. The human job moves:

| Before | Now |
|---|---|
| write the tests | **read the tests** |
| read all the code | read what each test pins |

A test that pins a bug passes, looks fine, and protects the bug from being fixed.

---

!!! question "💬 Which of these pin a bug?"

    An agent session wrote 18 characterization tests for `update()`, in
    `tests/five_steps_agent_test.cpp`. **All 18 pass.** Here are five. For each one: does it
    pin correct behaviour, a bug, or something you're unsure about?

    | Test | What it pins |
    |---|---|
    | `MovingIntoTheCellTheTailIsLeavingIsNotACollision` | the head moves onto the cell the tail just left; the game goes on |
    | `EatingFoodScoresGrowsAndMovesTwoCells` | eating: score 0 → 1, length 3 → 4, speed 1.0 → 0.95, head 10 → **12** |
    | `SecondMoveAfterEatingIsNotWallChecked` | eating in the last column: head at x = 20, off the board, game still running |
    | `KeepsMovingWhenAlreadyGameOver` | calling `update()` after game over still moves the snake |
    | `SpeedFactorBottomsOutAfterTwentyFourFoods` | speed stops changing after 24 foods, at 0.292 |

    ??? hint "Answer"
        | Test | Verdict | Why |
        |---|---|---|
        | `MovingIntoTheCell...` | correct | the tail moves away in the same tick; that's how snake works |
        | `EatingFoodScoresGrows...` | **correct and a bug, in one test** | score, length and speed are right; the jump to 12 is the double step |
        | `SecondMoveAfterEating...` | **bug** | alive outside the board |
        | `KeepsMovingWhenAlreadyGameOver` | **unsure** | the game loop never calls `update()` after game over, so no player sees it. Harmless today, a bug the day someone calls it. |
        | `SpeedFactorBottomsOut...` | **unsure** | the comment says speed increases "gradually"; nobody said where it should stop |

        **Look at the second test again.** The day someone fixes the double step, that test
        fails, and it also checks the score. The fixer can't tell whether they broke scoring
        or fixed the bug. Ask the agent to split it: one test per behaviour.

        **And the names.** The agent named what it saw: `...MovesTwoCells`,
        `...IsNotWallChecked`. It never said which ones are wrong. Its file has one comment
        instead: *"Do not 'fix' an expectation here without deciding to change the game's
        behaviour."* That decision is yours.

---
## Back to the pause key

The same five characterization tests, run before and after an agent added pause:

```
before: [  PASSED  ] 5 tests.
after:  [  PASSED  ] 5 tests.
```

**So the pause change is safe?**

??? hint "Answer"
    **Safe for `update()`. Nothing more.** The tests only pin `update()`. The agent
    changed four other functions, and no test watches them.

    Tests tell you about the code they cover. What to do about the rest is the next
    lecture.

---
## Plain prompt vs the five steps

The same request, on the same game, in two fresh agent sessions.

=== "Plain prompt"

    ```
    Add pause: P pauses, P again resumes.
    ```

=== "Five steps"

    1. *Don't write code.* List the functions that would have to change, and why.
    2. Which behaviour of `update()` could a test observe? *Don't write tests yet.*
    3. Write a test file that can call `update()` *without editing `snake.cpp`*.
    4. Write characterization tests for `update()`. Record what it does now.
    5. Add pause, with the decisions steps 1 and 2 made you take:

    ```
    Put the pause state in a new class. Don't change update(), Snake or Food.
    While paused, only P and Q do anything. Keep redrawing, with a PAUSED note.
    Change as few existing lines as you can. Then run the tests.
    ```

| | Plain prompt | Five steps |
|---|---|---|
| Prompts | 1 | 5 |
| Diff | +13 −1 | +12 −1 |
| Existing functions edited | 4 | 3 |
| Tests left behind | **0** | **18**, passing before and after |
| Bugs found and recorded | **0** | **5** |
| Who decided what keys do while paused | the agent, silently | you |

**The code came out about the same size. What did the five steps buy?**

??? hint "Answer"
    **Not better code.** The plain prompt even guessed the same pause behaviour this time.
    You only know that because the five steps made you decide it first.

    What changed is **what you know after the change**:

    - 18 tests to rerun on the next change to this file
    - five bugs on record instead of hidden: the double step, the second step skipping
      both the wall check and the self check, `update()` running after game over, and a
      capital R turning the snake right instead of restarting
    - every decision about pause made by a person, on purpose

    The plain prompt's change is the same size, and the next person to touch the file
    starts from zero again. The cost of the five steps: four extra prompts and some
    reading.

---
## The rule to keep

==Cover first, then let the agent modify.==

| Step | Ask |
|---|---|
| Before the change | What behaviour must not change? Is it pinned? |
| Reading agent tests | Is this pinned behaviour correct, a bug, or a question? |
| After the change | Did the tests pass because nothing broke, or because nothing was watched? |

---
## References:

1. *Working Effectively with Legacy Code* — Michael Feathers, ch. 1–2 (edit and pray, the change algorithm), ch. 13 (characterization tests)
2. [Approval Tests](https://approvaltests.com/) — snapshot-style characterization tests
3. [`Heer117/Snake_Game` @ e15f61a](https://github.com/Heer117/Snake_Game/blob/e15f61a1e3830dc100a64098e7b2c03adb46c217/snake.cpp) — the game used in this lecture; the tests were written for the course
