#### Quick recap
1. Refactoring = change structure, preserve behaviour
2. Five smells of **scattered behaviour**: Long Function, Duplicate Code,
   Divergent Change, Shotgun Surgery, Comments
3. Six techniques: Extract/Inline Function, Slide Statements, Replace Temp with
   Query, Split Loop, Split Phase

---


#### Agenda
1. Smells in the data (eight)
2. The refactorings that dissolve them (six)
3. What's still on the list — a look ahead to conditionals and inheritance

---
##### Mysterious Name

- The cheapest refactoring in the book, and the most skipped: **if you cannot
  name it, you do not understand it yet.**
- Two directions, both must hold:
    - could a reader guess the **body** from the name?
    - could a reader guess the **name** from the body?
- A name you have to explain in a comment is a rename you have not done.
- **Cure:** Rename Variable · Rename Field · Change Function Declaration.

!!! warning "Why it leads the list"
    Every other smell today is found by *reading names*. You cannot spot a Data
    Clump called `m1, m2` — the clump is invisible until the names tell the
    truth. Rename first; the other five smells then announce themselves.

---
##### Mysterious Name — the exercise

!!! question "💬 What does this class hold? Name every field."
    ```cpp
    struct node {
        int mi, mj;
        char data;
        node *next;
    };

    class Snake {
    public:
        node *head;
        int m1, m2;
        int dx, dy;
        char hch, bch;
        int cs, hs;
        // ...
        void changeIndex();
        bool onSnake(int x, int y);
        bool hitSnake(int x, int y, char arr[20][50]);
    };
    ```

    ??? hint "The answers (read off the call sites, not the declarations)"
        ```cpp
        struct Segment {
            int row, col;          // was mi, mj
            char glyph;            // was data
            Segment *next;
        };

        class Snake {
        public:
            Segment *head;
            int headRow, headCol;  // was m1, m2
            int rowStep, colStep;  // was dx, dy   ← note: NOT x/y deltas
            char headGlyph, bodyGlyph;   // was hch, bch
            int score, highScore;        // was cs, hs
            // ...
            void advanceBody();                          // was changeIndex
            bool occupiesCell(int row, int col);         // was onSnake
            bool isFatalCell(int row, int col, Board&);  // was hitSnake
        };
        ```
        - `hitSnake` **also checks obstacles** — the name is not merely vague, it
          is false. Renaming it forces you to notice it does two things.
        - `changeIndex` does not change an index. It shifts every body segment
          one step along the path.
        - Count the names for *one cell on the board*: `m1/m2`, `mi/mj`,
          `fi/fj` (in `Food`), and `x/y` in the parameter lists. **Four.**

---
##### Mysterious Name — in your code

!!! example "🔍 From your snake games"
    - [`Neel1585/Snake_Game` — `obstacle.cpp:28-46`](https://github.com/Neel1585/Snake_Game/blob/ac5dff594149f378b91a1b5a87d8d669ace3c13a/obstacle.cpp#L28-L46)
        - The exercise above is this file, unedited. Four names for one concept,
          and a **parameter list that inverts the axes**:

      ```cpp
      bool hitSnake(int x, int y, char arr[20][50]) {   // line 77
          node *tn = head->next;
          while (tn) {
              if (tn->mi == x && tn->mj == y) return true;
              else if (arr[x][y] == '#') return true;
              tn = tn->next;
          }
          return false;
      }
      ```

      It is called as
      [`snake.hitSnake(snake.m1, snake.m2, arr)`](https://github.com/Neel1585/Snake_Game/blob/ac5dff594149f378b91a1b5a87d8d669ace3c13a/obstacle.cpp#L202) —
      so the parameter named `x` receives a **row** and `y` receives a **column**,
      and the body indexes `arr[x][y]`, i.e. `arr[row][col]`. The code is correct.
      The *names* are the trap: the next person to call this — reasonably passing
      `(column, row)` — gets a silent wrong answer, not a compile error.

    - [`DeepSoni04/SnakeGame_Project` — `snake.cpp:9-28`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L9-L28)
        - `int x, y` is not the snake — it is the **head**. `dir` is an `int`
          holding one of five `#define`s. `nObs = 5` is the *live* obstacle count;
          the array bound is `20`; the name distinguishes neither.
        - The functions are `setup()`, `Draw()`, `Input()`, `logic()`. Two
          conventions in four names, in one file — Ousterhout's *pick conventions*
          (Lecture 3) failing at the smallest possible scale.

    **The contrasting good case**, from
    [`GarvModi18/SnakeGameProject` — `snake.cpp:145-179`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L145-L179):
    the *same* program shape, named properly — `headX, headY`,
    `isSpecialFruitOnScreen`, `SLOW_TIME_FRAMES`, and `enum eDirection` instead of
    `int dir`.
    **Naming and scoping are independent axes, and this repo got exactly one of
    them right.**

    ??? tip "🔧 How would you fix these?"
        - **Rename first, always.** It is the one refactoring with no behavioural
          risk, and it is diagnostic: `m1, m2` → `headRow, headCol` and the Data
          Clump becomes visible without you looking for it.
        - **When a rename is hard, that is the finding.** `hitSnake` resisted a
          name because it does two things. Split it, then name both.
        - **Fix the axes before anything else.** `(x, y)` meaning `(row, col)` is
          the kind of defect that survives every later refactoring, because each
          step preserves behaviour — including the wrong behaviour a future caller
          will trigger.
        - **Stop when** you can read a call site aloud and it is a true sentence.

---
##### Global Data

- Any code, anywhere, can read it. Any code, anywhere, can write it.
- The bug it produces has **no locality**: the write that broke you is nowhere
  near the read that failed. There is no interface to inspect, so there is no
  list of suspects.
- Same danger, decreasing radius: globals → class statics → class fields.
- **Cure:** Encapsulate Variable — get it behind functions, *then* narrow scope.

!!! warning "Why this is the load-bearing smell today"
    Global data is what makes Extract Function *look* like it worked. The
    functions come apart; the coupling does not. Lecture 4 ended three write-ups
    on exactly this note — *"otherwise the split is cosmetic."*

---
##### Global Data — the exercise

!!! question "💬 Two rules, one variable. Which one wins?"
    Both of these are inside the **same 120-line function**, 69 lines apart. The
    snake speeds up as you score: `gameSpeed` is the per-frame `Sleep()` in
    milliseconds, so *smaller is faster*.

    ```cpp
    // line 869 — runs every frame
    originalGameSpeed = max(120 - (score / 50) * 10, 10);
    if (!isSlowTimeActive) gameSpeed = originalGameSpeed;

    // line 938 — runs when a fruit is eaten (score += 10, so every 5 fruits)
    if (fruitsEaten % 5 == 0 && originalGameSpeed > 40) {
        originalGameSpeed -= 10;
        if (!isSlowTimeActive) gameSpeed = originalGameSpeed;
        speedLevel++;                      // ← displayed in the HUD
    }
    ```

    ??? hint "Trace it: what does the player see after 45 fruits?"
        - Line 869 runs **first, every frame**, and recomputes `originalGameSpeed`
          from `score` alone. Whatever line 938 wrote is discarded one frame later.
        - Below score 400 the two rules happen to agree — both step 10 ms per 5
          fruits from 120 — so nothing looks wrong.
        - At score 400, `originalGameSpeed` reaches 40. Line 938's guard
          `originalGameSpeed > 40` now fails forever, so **`speedLevel` freezes**.
        - Line 869 has no such guard. It keeps going: 20 ms at score 500, and its
          floor of 10 ms at score 600.
        - **Result:** past ~45 fruits the HUD reads `Speed Level: 9` permanently
          while the game accelerates to **four times** that speed. Two authors of
          the same rule, 69 lines apart, neither aware of the other.
        - Nothing about either snippet is wrong *locally*. That is the point.

---
##### Global Data — in your code

!!! example "🔍 From your snake games"
    | Repo | mutable variables at file scope |
    |:--|--:|
    | [`GarvModi18/SnakeGameProject`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L43-L180) | **31** |
    | [`DeepSoni04/SnakeGame_Project`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L15-L28) | 14 |
    | [`Hardikk69/snake-game`](https://github.com/Hardikk69/snake-game/blob/c4cda77becad9d4b77512a7be1d254287ffc9d47/Snake-game.cpp#L9-L17) | 14 |
    | `Shiroilt/solo-leveling-chapter-snake-monster` | 8 |

    - **`GarvModi18/SnakeGameProject` — 31 globals, and the speed bug above.**
      This is the Lecture 4 poster child for Divergent Change *and* Shotgun
      Surgery. The globals are **why**. `gameSpeed` and `originalGameSpeed` are
      written at **seven sites in three functions** —
      [159](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L159) ·
      [677-678](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L677-L678) ·
      [852-854](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L852-L854) ·
      [866](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L866) ·
      [869-870](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L869-L870) ·
      [938-939](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L938-L939) —
      and the guard `if (!isSlowTimeActive)` is copy-pasted at three of them. The
      initial value `= 110` at line 159 is dead: line 869 overwrites it with 120
      on the first frame of every game.

    - **`DeepSoni04/SnakeGame_Project` — the whole program has no parameters.**
      Apart from `main`, every function in
      [`snake.cpp`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp)
      is declared `void f()` — `loadHighScore`, `saveHighScore`, `setup`, `Draw`,
      `Input`, `logic` — and not one of them takes an argument. **The globals
      *are* the parameter list**; they are just never written down, so no
      signature tells you what
      [`logic()`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L155-L199)
      touches. It reads or writes **thirteen of the fourteen** (everything except
      `highscore`), in 45 lines.

      And because the interface is invisible, the only way the author could make a
      variant was to copy the file. The same block is declared four times over:

      | | `snake.cpp` | `deepsnake.cpp` | `SnakeBite.cpp` | `SNAKE_Main1.cpp` |
      |:--|:--|:--|:--|:--|
      | `gameOver` | `bool` | `bool` | `bool` | **`int`** |
      | `height` | 20 | 20 | **17** | — |
      | `nObs` | 5 | **4** | — | — |

      Global data plus versioning-by-file-copy (Lecture 4) — and the copies have
      already drifted in the value *and the type*.

    ??? tip "🔧 How would you fix these?"
        - **Encapsulate Variable first.** `int gameSpeed;` → `getGameSpeed()` /
          `setGameSpeed(int)`. You get nothing but a greppable list of writers and
          one place to breakpoint — which is precisely what the speed bug needed.
        - **Then let the accessors carry the variable home.** `gameSpeed`,
          `originalGameSpeed`, `slowTimeDuration`, `isSlowTimeActive` and
          `speedLevel` are all touched by the same three functions: that is a
          `SpeedController`, and the two contradictory rules end up in one body
          where they cannot avoid each other.
        - **Do not wrap all 31 in one `struct Globals` and pass it everywhere.**
          That is a rename, not a refactoring — same reach, longer signatures.
          Split by *who reads it*.
        - **Stop when** you can answer "who can change the speed?" by reading one
          class.

    !!! note "Smaller radius, same smell"
        [`Dazzling-Darshan/SnakeByte`](https://github.com/Dazzling-Darshan/SnakeByte/blob/17bc256926a95aa647314649331fe78d4b52c41b/game.h#L51-L138)
        has **no file-scope variables at all** — its fourteen live as fields of
        `class Game`. That is genuinely better: the blast radius is one class
        instead of one program. But Lecture 4 already showed the bill for it under
        Divergent Change — *five reasons to change, one class*. Shrinking the
        radius is progress; it is not the finish line.

    !!! note "The order matters"
        This is why Global Data comes before Data Clumps and Long Parameter List
        rather than after. You cannot introduce a parameter object for data that
        is never passed as a parameter.

---
##### Mutable Data

- Updating one field for one purpose silently changes what another reader sees.
- Sibling smells: variables reused for two things, values recomputed vs. stored.
- **Cures:** Split Variable (one variable, one job) · Replace Derived Variable
  with Query (don't store what you can compute) · Remove Setting Method ·
  Combine Functions into Transform.

!!! question "💬 Beat the high score. What gets written to the file?"
    Every line below is the complete lifetime of `highScore` in this program —
    all five references, in one 146-line file.

    ```cpp
    int x,y,fruitX,fruitY,score,highScore;          // line 11

    void loadHighScore(){
        ifstream f("highscore.txt");
        if(f.good()) f>>highScore;                  // line 21
        else highScore=0;
    }

    void saveHighScore(){
        ofstream f("highscore.txt");
        f<<highScore;                               // line 27
    }

    // ...in the game loop:
    cout<<"Score: "<<score<<"   HighScore: "<<highScore;   // line 78
    if(score>highScore) saveHighScore();                   // line 110
    ```

    ??? hint "The answer: the old high score, unchanged, forever"
        - `highScore` is **assigned in exactly one place** — line 21/22, reading
          the file at startup.
        - Line 110 correctly detects that you beat it, and calls `saveHighScore()`.
        - `saveHighScore()` writes `highScore`. Nobody ever wrote `score` into it.
        - **The file is rewritten with the value it already had.** The high score
          in this game can never go up.
        - The missing line is `highScore = score;`. But notice *why* it went
          missing: `highScore` is **derived** state — it is `max(everything you
          ever scored)` — stored as a free-floating mutable variable that any code
          path is free to forget. Fowler's cure is named for exactly this:
          **Replace Derived Variable with Query.**

---
##### Mutable Data — in your code

!!! example "🔍 From your snake games"
    - [`Hardikk69/snake-game` — `Snake-game.cpp:11-110`](https://github.com/Hardikk69/snake-game/blob/c4cda77becad9d4b77512a7be1d254287ffc9d47/Snake-game.cpp#L11-L110)
        - The high-score bug above, in full. It is not a typo — it is what
          happens when a value that should be computed is stored instead.

    - **The same fact stored twice.** The same file keeps the snake's head in
      **two representations at once**
      ([`:117-126`](https://github.com/Hardikk69/snake-game/blob/c4cda77becad9d4b77512a7be1d254287ffc9d47/Snake-game.cpp#L117-L126)):

      ```cpp
      for(int i=snake.size()-1;i>0;i--) snake[i]=snake[i-1];
      snake[0]={x,y};                              // globals x,y copied into the body

      for(int i=1;i<snake.size();i++){             // ...so this loop must start at 1
          if(snake[i].first==x&&snake[i].second==y) gameOver=true;
      }
      ```

      `x, y` **is** the head; `snake[0]` is a copy of it, refreshed by hand every
      tick. The self-collision loop starts at `i=1` *only* because of that
      duplication — the bound encodes a fact about a different line. Change how
      `snake[0]` is maintained and the loop is silently wrong.
      [`DeepSoni04/SnakeGame_Project`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L156-L169)
      has the identical arrangement with `tailX[0], tailY[0]`.

    - **Recomputed *and* stored.** `GarvModi18/SnakeGameProject`'s
      `originalGameSpeed` is the third variety: derived from
      `score` on line 869, then separately decremented on line 938. Two writers,
      one variable, no coordination — which is the definition of the smell.

    ??? tip "🔧 How would you fix these?"
        - **Ask of every variable: is this a fact, or a conclusion?** Conclusions
          should be functions. `int highScore()` reading `max(stored, score)`
          cannot be forgotten, because there is nothing to forget to write.
        - **snake-game** → delete `x, y`. The head *is* `snake.front()`. One
          representation, no sync step, and the `i=1` loop bound becomes
          `snake.begin()+1` for an honest reason.
        - **Split Variable** where one name serves two jobs; **Remove Setting
          Method** where a field should be set once at construction and never
          again.
        - **Order:** do this *after* Encapsulate Variable. You cannot replace a
          variable with a query until all reads go through one place.

    !!! note "Why these bugs survive testing"
        All three programs run. The high score displays, the snake collides, the
        speed increases. Each defect needs a *second* interaction — beat the
        score twice, change the body representation, score past 400 — before it
        shows. That is what "makes the next change expensive" means in practice.

---
##### Data Clumps

- The same three or four items travelling together — as fields, as parameters,
  in that order, again and again.
- **The test:** delete one of them. Do the others still mean anything? If not,
  they are one concept wearing three names.
- **Cure:** Extract Class (if they're fields) → Introduce Parameter Object /
  Preserve Whole Object (once they are).
- The payoff is not shorter signatures. It's that behaviour now has somewhere to
  live — `Position` gets `isInsideBoard()`, and Feature Envy dissolves with it.

---
##### Data Clumps — in your code

!!! example "🔍 From your snake games"
    - [`Nevil-Nandasana/Snake-Game` — `snake_game.cpp`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp) —
      **`originRow, originCol` appear 63 times**, threaded as a pair through nine
      signatures:

      ```cpp
      void drawCell(const GameConfig& cfg, int originRow, int originCol, Vec v, const string& glyph);              // 381
      void drawBorder(const GameConfig& cfg, int originRow, int originCol);                                        // 385
      void drawEnhancedUI(const GameConfig& cfg, int originRow, int originCol, const GameState& st, const LayoutInfo& layout);  // 398
      void showPauseDialog(const GameConfig& cfg, int originRow, int originCol);                                   // 450
      void showLevelUpNotification(const GameConfig& cfg, int originRow, int originCol, int newLevel);             // 459
      void replayGame(const GameConfig& cfg, const GameState& st, int originRow, int originCol, const LayoutInfo& layout);      // 471
      void drawBoardInitial(const GameConfig& cfg, const GameState& st, int originRow, int originCol);             // 697
      bool showGameOverScreen(const GameConfig& cfg, int originRow, int originCol, GameState& st, const LayoutInfo& layout);    // 750
      void checkLevelUp(GameConfig& cfg, GameState& st, int originRow, int originCol, const LayoutInfo& layout);   // 821
      ```

      **Now the twist.** The parameter object already exists — and it already
      holds the clump
      ([`:188-195`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L188-L195)):

      ```cpp
      struct LayoutInfo {
          int originRow;
          int originCol;
          int panelCol;
          int termRows, termCols;
          bool compactMode;
      };
      ```

      So in four of those nine signatures, `originRow` and `originCol` are passed
      **twice** — once loose, once inside `layout`. `runGame`
      ([`:837-841`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L837-L841))
      unpacks them on entry purely to thread them onward:

      ```cpp
      void runGame(GameConfig cfg, GameState& st, LayoutInfo& layout) {
          int originRow = layout.originRow;
          int originCol = layout.originCol;
      ```

      and at
      [`:1048`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L1048)
      the caller does it inline, passing the object and its own fields side by
      side in one call:

      ```cpp
      playAgain = showGameOverScreen(cfg, layout.originRow, layout.originCol, st, layout);
      ```

      This is **Preserve Whole Object**'s motivating example, verbatim, in your
      code. The refactoring here is not "invent an abstraction" — it is *stop
      taking the existing one apart*.

    - **Parallel arrays are the same smell, differently spelled.**
      [`DeepSoni04/SnakeGame_Project` — `snake.cpp:19-25`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L19-L25):
      `tailX[100], tailY[100]` and `obsX[20], obsY[20]`. Two arrays that must be
      indexed together, resized together and copied together are **one array of
      pairs** — plus `nTail` and `nObs`, two counters that exist only because the
      arrays cannot report their own length.

    ??? tip "🔧 How would you fix these?"
        - **Snake-Game** → delete the two parameters from all nine signatures and
          read `layout.originRow` inside. Zero new types; eighteen parameters
          gone. Then `drawCell` can take `layout` and lose `cfg` too.
        - **SnakeGame_Project** → `struct Cell { int row, col; };` then
          `vector<Cell> tail`. `nTail` disappears into `.size()` — one fewer
          variable that can disagree with reality.
        - **Watch what happens next.** Once `Cell` exists, `wrapAround()` and
          `collidesWith()` have an obvious owner. The clump was hiding a class,
          and the class was hiding the behaviour.
        - **Stop when** the group no longer appears in any signature — not when
          you have merely named it.

---
##### Primitive Obsession

- Money as `float`, a phone number as `string`, a direction as `int`, a
  coordinate as two `int`s.
- Primitives carry no rules. `dir = 7` compiles. `row + col` compiles.
- **Cure:** Replace Primitive with Object · Replace Type Code with Subclasses
  (Lecture 7).
- Connection to Lecture 3: a primitive is the **shallowest possible module** —
  zero behaviour hidden behind the interface. Wrapping it is where depth comes
  from.

---
##### Primitive Obsession — in your code

!!! example "🔍 From your snake games"
    - **`int dir` as a type code.**
      [`DeepSoni04/SnakeGame_Project` — `snake.cpp:9-21`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L9-L21):

      ```cpp
      #define STOP 0
      #define LEFT 1
      #define RIGHT 2
      #define UP 3
      #define DOWN 4
      // ...
      int dir;
      ```

      Five legal values out of four billion, and the compiler will not stop you
      writing `dir = 7`, `dir + 1`, or `dir = score`. The type says "integer"; the
      truth is "one of five", and the `switch (dir)` at
      [`:171`](https://github.com/DeepSoni04/SnakeGame_Project/blob/2807e4c6bf25e6b723d3a7a8ca4d828a3587541c/snake.cpp#L171-L176)
      exists purely to translate the integer back into meaning. Because this file
      was copied four times, so was the switch: **eight sites
      across the repo encode the same five constants.**

    - **`pair<int,int>` is a better primitive, not an abstraction.**
      [`maitry4/code_crafters` — `gameLogic.h:181-187`](https://github.com/maitry4/code_crafters/blob/8a9aa60ccbbffc70f77456fc796cc8d347fde96d/gameLogic.h#L181-L187)
      gives the cleanest proof — the author wraps the coordinate, then
      **immediately unwraps it because the wrapper is unreadable**:

      ```cpp
      void initialize(pair<int, int> startPos, int length, Direction direction, Board& board) {
          body.clear();
          growthPending = 0;

          int startRow = startPos.first;      // ← the rename the type should have done
          int startCol = startPos.second;
      ```

      `.first` and `.second` carry no meaning, so every reader pays a translation
      tax at every use. `pair` bought type-safety and spent it all on naming.
      Nine repos in the corpus use `pair<int,int>` as a board coordinate.

    **The contrasting good case**, from
    [`Nevil-Nandasana/Snake-Game` — `snake_game.cpp:60-63`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L60-L63):

    ```cpp
    struct Vec { int r, c; };
    enum Dir { UP=0, RIGHT=1, DOWN=2, LEFT=3 };
    enum FoodType { NORMAL, GOLDEN, POISON };
    enum PowerUpType { SLOW_MOTION, INVINCIBILITY, MAGNET, NONE };
    ```

    Named fields, named values — and behaviour has somewhere to go:
    [`step(Vec v, Dir d)`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L226-L232)
    and `isOpposite(Dir a, Dir b)` read like the rules they implement. Note
    though that `Vec` is still a *data* struct — `step` is a free function, not a
    method.

    ??? tip "🔧 How would you fix these?"
        - `int dir` → `enum class Direction` → then a `Direction` that knows its
          own `delta()`. Each step deletes a `switch`; count them before you
          decide it isn't worth it.
        - `pair<int,int>` → `struct Cell { int row, col; };`. The rename alone
          pays: `food.first` versus `food.row`. Then move `isInsideBoard()`,
          `wrapAround()` and `operator==` onto it.
        - **Where it connects to Lecture 3:** a bare primitive is the *shallowest
          possible module* — nothing hidden, no behaviour, all interface. Wrapping
          it is where depth comes from, which is why this refactoring pays back far
          more than the typing it costs.
        - **The honest objection** — "this is over-engineering for a snake game."
          Sometimes true. The test is not the size of the program, it is *how many
          places already know the rules of this primitive*. Eight sites decoding
          `dir` means eight places that must be edited together the day you add a
          diagonal move.

---
##### Long Parameter List

- Historically the price of avoiding global data. Both extremes are bad; the
  middle is objects with clear ownership.
- Four families, four cures:
    - one param derivable from another → **Replace Parameter with Query**
    - several params from one object → **Preserve Whole Object**
    - params that always travel together → **Introduce Parameter Object**
    - a `bool` that switches behaviour → **Remove Flag Argument**
- **Boolean parameters deserve their own grudge:** `render(board, true)` — true
  *what*? The flag means the function is two functions.

---
##### Long Parameter List — the exercise

!!! question "💬 Read this call. What does it ask for?"
    ```cpp
    vector<Point> blocks = generateBlocks(W, H, 6, 3, 6, 6);
    ```

    Four bare integers in a row. Without scrolling to the declaration, say what
    each `6` means — and whether swapping the last two would change anything.

    ??? hint "You cannot, and that is the finding"
        ```cpp
        vector<Point> generateBlocks(int w, int h, int clusterCount,
                                     int blockMin, int blockMax, int spacingMin)
        ```
        `6` clusters, of `3`–`6` blocks each, `6` apart. Swapping the last two
        gives `blockMax = 6, spacingMin = 6` → unchanged here **by coincidence**,
        because three of the four arguments happen to be `6`. The next reader gets
        no warning from the compiler and no warning from the call site.

        ```cpp
        // After Introduce Parameter Object
        struct ObstacleSpec { int clusters, minBlocks, maxBlocks, minSpacing; };
        auto blocks = generateBlocks(board, ObstacleSpec{6, 3, 6, 6});
        ```
        Still four sixes — but now each one is *named at the point it is written*,
        and the struct is a place to put `validate()`.

---
##### Long Parameter List — in your code

!!! example "🔍 From your snake games"
    All three from
    [`divyesh-dandwani/Snake-Game-CPP` — `snake_gamebox.cpp`](https://github.com/divyesh-dandwani/Snake-Game-CPP/blob/205fd2aea17acea256a39ee3edfbd822bf0b6440/snake_gamebox.cpp),
    one file, three different families of the smell:

    | Line | Signature | Family |
    |:--|:--|:--|
    | [145](https://github.com/divyesh-dandwani/Snake-Game-CPP/blob/205fd2aea17acea256a39ee3edfbd822bf0b6440/snake_gamebox.cpp#L145) | `generateBlocks(int w, int h, int clusterCount, int blockMin, int blockMax, int spacingMin)` | Introduce Parameter Object |
    | [240](https://github.com/divyesh-dandwani/Snake-Game-CPP/blob/205fd2aea17acea256a39ee3edfbd822bf0b6440/snake_gamebox.cpp#L240) | `drawBox(int w, int h, int ox, int oy, int color)` | Data Clump — **`Point` already exists** |
    | [342](https://github.com/divyesh-dandwani/Snake-Game-CPP/blob/205fd2aea17acea256a39ee3edfbd822bf0b6440/snake_gamebox.cpp#L342) | `showScoreboard(int score, int speed, int highScore, const Theme &t, bool rainbow)` | Preserve Whole Object **+ flag argument** |

    - **`drawBox`** takes `ox, oy` as loose ints while
      [`struct Point { int x, y; };`](https://github.com/divyesh-dandwani/Snake-Game-CPP/blob/205fd2aea17acea256a39ee3edfbd822bf0b6440/snake_gamebox.cpp#L22)
      sits at line 22 of the same file and is the return type of `generateBlocks`.
      Same pattern as Nevil's `LayoutInfo`: **the abstraction exists and is walked
      past.**

    - **`showScoreboard`** is the flag-argument case. `score, speed, highScore`
      are three quarters of the game's state passed individually, and the trailing
      `bool rainbow` selects between two rendering behaviours. Read the call:

      ```cpp
      showScoreboard(score, speed, highScore, cur, rainbowMode);   // line 428
      ```

      Nothing at that call site tells you what the last argument does. A reader
      must open the function to discover there is an `if (rainbow)` branch inside
      — which is to say, **two functions sharing one name.**

    ??? tip "🔧 How would you fix these?"
        - **`generateBlocks`** → `generateBlocks(board, ObstacleSpec{...})`. Six
          parameters → two, and the spec becomes a thing you can name, default,
          serialise and validate.
        - **`drawBox`** → `drawBox(Point origin, int w, int h, int color)`. Use the
          type you already wrote.
        - **`showScoreboard`** → `showScoreboard(state, theme)` plus
          `showRainbowScoreboard(state, theme)`. Remove Flag Argument turns an
          unreadable `true` at the call site into a readable verb.
        - **Do not** invent a parameter object for parameters that are genuinely
          independent — that manufactures a clump instead of finding one. The test
          is whether they always travel *together*.
        - **The trade-off is real.** Fowler's own framing: long parameter lists
          were the historical price of avoiding global data. You have now seen both
          failure modes in this lecture — 31 globals on one side, six positional
          ints on the other. The answer is neither; it is objects with clear
          ownership.

---
##### Feature Envy & Data Class

- **Feature Envy:** a function in module A spends its time calling getters on
  module B. The function is in the wrong place.
- **Data Class:** a class that is nothing but fields and accessors — data that
  other people operate on.
- They are the same defect seen from either end: **behaviour and the data it
  needs are in different modules.**
- Heuristic: *put together the things that change together* — and the strongest
  signal of "changes together" is "reads the same data".
- **Cure:** Move Function (whole) · Extract Function then Move Function (part) ·
  Encapsulate Record / Encapsulate Collection to stop the leak at source.

!!! question "💬 Where does this function belong?"
    ```cpp
    void checkLevelUp(GameConfig& cfg, GameState& st, int originRow, int originCol,
                      const LayoutInfo& layout) {
        int scoreForNextLevel = cfg.currentLevel * 200;
        if (st.score >= scoreForNextLevel && st.score > st.lastLevelUpScore
                                          && cfg.currentLevel < cfg.maxLevel) {
            cfg.currentLevel++;
            st.lastLevelUpScore = st.score;
            // ...then eight lines of redrawing
        }
    }
    ```

    ??? hint "It belongs to the data it cannot stop touching"
        - Every value in the decision comes from `cfg` or `st`. The function owns
          **nothing** — it is a rule about `GameState` that happens to be stored
          outside `GameState`.  
        - It also does two jobs: *decide whether to level up* and *redraw the
          board*. Extract Function first, then move only the decision.  
        - After: `if (st.readyForNextLevel(cfg)) { st.advanceLevel(cfg); redraw(...); }`  
          — and the `lastLevelUpScore` field can finally become private, because
          only one place still reads it.  

---
##### Feature Envy & Data Class — in your code

!!! example "🔍 From your snake games"
    - [`Nevil-Nandasana/Snake-Game` — `snake_game.cpp:118-135`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L118-L135)
        - **21 fields, 0 methods, 14 free functions that take it by reference** —
          `initGameState`, `placeFood`, `spawnPowerUp`, `spawnDynamicObstacle`,
          `updateDynamicObstacles`, `updateGameSpeed`, `loadLevelObstacles`,
          `checkLevelUp`, `drawBoardInitial`, `drawEnhancedUI`, `showMenu`,
          `showGameOverScreen`, `replayGame`, `runGame`.

      ```cpp
      struct GameState {
          deque<Vec> snake;
          Dir dir = RIGHT;
          Vec food{-1, -1};
          FoodType foodType = NORMAL;
          set<pair<int,int>> obstacles;
          vector<DynamicObstacle> dynamicObstacles;
          vector<PowerUp> powerUps;
          PowerUpType activePowerUp = NONE;
          int powerUpTimer = 0;
          bool gameOver = false, quit = false, paused = false;
          int score = 0, moves = 0;
          int goldenApplesEaten = 0, poisonApplesEaten = 0;
          int segmentsToGrow = 0, segmentsToShrink = 0;
          int lastLevelUpScore = 0;
          chrono::steady_clock::time_point startTime;
          vector<pair<Dir, Vec>> moveHistory;
      };
      ```

      Every rule in the game — growth, collision, scoring, power-up expiry,
      level-up — lives in a free function that takes `GameState& st` and reaches
      in. Nothing can be private, because everything is somebody else's business.
      This is the **same repo** that got Primitive Obsession right:
      it has `Vec`, `Dir`, `FoodType`. **It built the vocabulary and then wrote all
      the sentences somewhere else.**

    - **`Vec` is the miniature of the same thing.** `struct Vec { int r, c; };`
      with `step(Vec, Dir)` and `isOpposite(Dir, Dir)` as free functions
      ([`:226-232`](https://github.com/Nevil-Nandasana/Snake-Game/blob/d82a114b76f75d447774a95577ff8eed226648a0/snake_game.cpp#L226-L232)).
      Those two functions are the *only* things that know what a `Vec` means, and
      neither is on it.

    - **The other end of the same defect.** In
      `DeepSoni04/SnakeGame_Project` there are no classes at all, so there is no
      envy to detect — the data and the behaviour are equally global. Feature Envy
      is a smell you can only *have* once you have started separating things. It
      shows up in the repos that are trying.

    ??? tip "🔧 How would you fix these?"
        - **Ask: which data does this function touch most?** Move it there. If the
          answer is "two modules equally", Extract Function first and move only the
          envious half.  
        - **Combine Functions into Class** is the bulk operation: take the 14 free
          functions on `GameState`, move them in, and the struct becomes a class.  
        - **A Data Class is not automatically wrong.** DTOs at a boundary, config
          records, `Vec` as a pure value — all fine. It is a smell when the *rules
          about that data* live elsewhere.  
        - **Test you did it right:** fields you moved past can now be `private`.
          `lastLevelUpScore`, `segmentsToGrow`, `powerUpTimer` are pure bookkeeping  
          — no outsider should ever see them. **If nothing became private, nothing
          moved.**  

---
#### Common refactoring techniques

Six techniques, in the order you would actually apply them to the code above.
Every one is a **behaviour-preserving** step: compile and test after each.

---
##### Rename Variable / Change Function Declaration

!!! question "💬 Same code, different names. Which bug is now visible?"
    ```javascript
    // before
    function circum(r) { return 2 * Math.PI * r; }
    ```

    ??? hint "After Rename"
        ```javascript
        function circumference(radius) { return 2 * Math.PI * radius; }
        ```
        - `circum` could have been circumference *or* circumscribe. `r` could have
          been radius *or* rate. Nothing changed except that the reader stopped
          guessing.  
        - In `hitSnake(int x, int y)` earlier, the rename to `(int row, int col)`
          is what makes the axis inversion visible at every call site.  

- **Mechanics**
    1. If the name is used outside your codebase, treat it as a published API and
       do it in two steps: add the new declaration, make the old one delegate,
       deprecate, remove later.
    2. Otherwise: rename → compile → test. Let the compiler find the references.
    3. For a function, Change Function Declaration also covers adding, removing
       and reordering parameters — same mechanics.
- **Principle:** *the cheapest refactoring in the book; do it the moment the name
  stops being true.*

---
##### Encapsulate Variable

!!! question "💬 If the snake speeds up unexpectedly, who can change this?"
    ```javascript
    let gameSpeedMs = 120;   // per-frame delay; smaller is faster
    // ...referenced from anywhere, written from anywhere
    ```

    ??? hint "After Encapsulate Variable"
        ```javascript
        let gameSpeedMs = 120;

        export function gameSpeed() {
            return gameSpeedMs;
        }

        export function setGameSpeed(delayMs) {
            gameSpeedMs = Math.max(delayMs, 10);
        }
        ```
        - Every writer is now explicit: grep `setGameSpeed(` and you have the
          suspect list.  
        - Logging, breakpoints, validation or locking now live in one place.  
        - Once all access goes through these functions, `gameSpeedMs` can move
          into the class or module that actually owns speed control.  
        - If the variable were a record rather than a number, return a copy or
          expose narrower update operations so callers cannot mutate shared
          state behind your back.  

- **Mechanics:** add the accessors → route every reference through them →
  restrict direct access → *then* consider copying on read.
- **Why it comes first:** you cannot move, split or delete data until you control
  who touches it. This is the step that turns the 31 globals from an unbounded
  problem into a finite list.
- **Principle:** *encapsulate to make the next refactoring possible.*

---
##### Introduce Parameter Object

!!! question "💬 These two always travel together. What are they?"
    ```javascript
    function amountInvoiced(startDate, endDate) { /* ... */ }
    function amountReceived(startDate, endDate) { /* ... */ }
    function amountOverdue(startDate, endDate)  { /* ... */ }
    ```

    ??? hint "After Introduce Parameter Object"
        ```javascript
        class DateRange {
            constructor(start, end) { this._start = start; this._end = end; }
            get start() { return this._start; }
            get end()   { return this._end; }
            includes(aDate) { return aDate >= this._start && aDate <= this._end; }
        }

        function amountInvoiced(aDateRange) { /* ... */ }
        ```
        - The parameters shrink — but the point is `includes()`. Every function
          that used to hand-write `date >= start && date <= end` now calls one
          method that cannot be written backwards.

- **Mechanics:** create the class → add it as a parameter → move callers over one
  at a time → remove the old parameters → **then hunt for behaviour to move in.**
- **Step 5 is the whole point.** Stopping at "fewer parameters" collects the cost
  and skips the payoff.
- **Principle:** *a recurring group of parameters is a class nobody has written
  yet.*

---
##### Replace Primitive with Object

!!! question "💬 `"high"` is a string. What stops `"HIGH"`, or `"hgih"`?"
    ```javascript
    highPriorityCount = orders.filter(o => "high" === o.priority
                                        || "rush" === o.priority).length;
    ```

    ??? hint "After Replace Primitive with Object"
        ```javascript
        class Priority {
            constructor(value) {
                if (!Priority.legalValues().includes(value))
                    throw new Error(`<${value}> is invalid for Priority`);
                this._value = value;
            }
            toString()   { return this._value; }
            get _index() { return Priority.legalValues().indexOf(this._value); }
            static legalValues() { return ['low', 'normal', 'high', 'rush']; }
            higherThan(other) { return this._index > other._index; }
        }

        highPriorityCount = orders.filter(o => o.priority.higherThan(new Priority("normal"))).length;
        ```
        - Validation now happens **once, at construction**, instead of never.
        - Ordering (`higherThan`) had nowhere to live before. It does now.

- **Mechanics:** encapsulate the field → create the wrapper class → make the
  getter return the object → move validation and comparison in.
- **Lecture 3 connection:** this is precisely how a *shallow* module gets deeper.
  `Priority` hides four legal values, an ordering and a validation rule behind a
  two-method interface.

---
##### Combine Functions into Class

!!! question "💬 Three functions, one argument. What is missing?"
    ```javascript
    function base(aReading)     { /* ... */ }
    function taxableCharge(aReading) { /* ... */ }
    function calculateBaseCharge(aReading) { /* ... */ }
    ```

    ??? hint "After Combine Functions into Class"
        ```javascript
        class Reading {
            constructor(data) { Object.assign(this, data); }
            get baseCharge()    { /* ... */ }
            get taxableCharge() { /* ... */ }
        }
        ```
        - Every function that takes the same record as its first argument is a
          method wearing a disguise.
        - The shared intermediate results (`baseCharge` used by `taxableCharge`)
          stop being recomputed or passed around.

- **Mechanics:** Encapsulate Record first → move each function in one at a time →
  extract any leftover shared body into its own method.
- **This is the bulk cure for Data Class + Feature Envy** — the two smells from
  the Feature Envy & Data Class pair dissolve together, because they were always
  one smell.
- **Alternative:** *Combine Functions into Transform* does the same job when the
  source data must stay immutable — it returns an enriched copy rather than
  owning the original. Prefer the class when the data is updated; prefer the
  transform in a read-only pipeline.

---
##### Move Function / Move Field

!!! question "💬 Which class does this belong to?"
    ```javascript
    class Account {
        get bankCharge() { /* ... */ }
        get overdraftCharge() {
            if (this.type.isPremium) {
                const baseCharge = 10;
                if (this.daysOverdrawn <= 7) return baseCharge;
                return baseCharge + (this.daysOverdrawn - 7) * 0.85;
            }
            return this.daysOverdrawn * 1.75;
        }
    }
    ```

    ??? hint "After Move Function"
        ```javascript
        class AccountType {
            overdraftCharge(daysOverdrawn) {
                if (this.isPremium) {
                    const baseCharge = 10;
                    if (daysOverdrawn <= 7) return baseCharge;
                    return baseCharge + (daysOverdrawn - 7) * 0.85;
                }
                return daysOverdrawn * 1.75;
            }
        }

        class Account {
            get overdraftCharge() { return this.type.overdraftCharge(this.daysOverdrawn); }
        }
        ```
        - Every branch asked `this.type` a question. The rule was about the
          *account type*, not the account.
        - Adding a new account type is now one class, not one more `if`.

- **Mechanics:** examine everything else the function uses → decide whether those
  should move too → copy it to the target → turn the original into a delegator →
  test → inline the delegator once no caller needs it.
- **Move Field** is the same dance for data: create the field on the target,
  update the accessors to point at it, then remove the old field.
- **Principle:** *put the function where its data is. If you cannot tell where
  that is, the data is the problem — go back to Encapsulate Variable.*
- **Principle:** *put the function where its data is; if you can't tell where
  that is, the data is the problem.*

---
#### Where this leaves us

| Lecture 4 said | Lectures 5-6 say |
|:--|:--|
| Extract Function | …but you can't extract past shared globals |
| Split Phase | …the intermediate structure *is* the parameter object |
| Shotgun Surgery | …scattered edits usually track scattered data |
| Comments as deodorant | …`// x is actually the head row` is a Rename |

- The five smells of Lecture 4 and the eight here are one dependency
  graph. **Encapsulate Variable is usually the first move, not Extract Function.**

---
##### The order to do them in

Refactorings unlock each other. Doing them out of order is why the first attempt
usually stalls.

```
1. Rename                    ← free, no risk, and it reveals everything below
2. Encapsulate Variable      ← turns "anyone can touch it" into a finite list
3. Split Variable /
   Replace Derived Variable  ← one variable, one job; stop storing conclusions
4. Extract Class /
   Introduce Parameter Object ← the clump becomes a type
5. Replace Primitive with Object
6. Move Function / Combine Functions into Class
                             ← behaviour finally joins its data
7. Extract Function          ← Lecture 4's step — it works *now*
```

!!! tip "Why Extract Function is last, not first"
    It is the refactoring everyone reaches for on day one, and it is the one that
    silently fails on this code. Extracting a function out of `logic()` in
    `DeepSoni04/SnakeGame_Project` produces a function with no parameters that
    still reads thirteen globals. You have moved lines, not dependencies. Steps
    1–6 are what make step 7 mean something.

!!! warning "One caveat: you need tests first"
    Every step above is *behaviour-preserving in principle*. Without a test you
    cannot know you preserved it — and several of these repos contain latent bugs
    (the high score that never saves, the speed level that freezes) that a
    refactoring will faithfully carry forward. **Fowler's Chapter 4 comes before
    Chapter 6 for a reason**, and it is where Module 3 of this course picks up.

---
#### Still on the list

Fowler's Chapter 3 has 24 smells. Lecture 4 covered 5, today 8 — **13 down, 11
to go**. Coming in **Lecture 7**:

- **Conditional logic** — Repeated Switches → Replace Conditional with
  Polymorphism; Introduce Special Case; Guard Clauses.
- **Inheritance** — Refused Bequest, Alternative Classes with Different
  Interfaces.
- **Too much / too little indirection** — Message Chains, Middle Man, Insider
  Trading, Lazy Element, Speculative Generality, Temporary Field.
- **Size** — Large Class (the Lecture 4 repos qualify twice over).
- **Loops** — Replace Loop with Pipeline.

---
#### References

1. Chapter 3, [Refactoring, Second edition](https://martinfowler.com/books/refactoring.html) — Bad Smells in Code (Mysterious Name p.72, Long Parameter List p.74, Global Data p.74, Mutable Data p.75, Feature Envy p.77, Data Clumps p.78, Primitive Obsession p.78, Data Class p.83)
2. Chapter 6 — Introduce Parameter Object (140), Combine Functions into Class (144)
3. Chapter 7 — Encapsulate Record (162), Encapsulate Collection (170), Replace Primitive with Object (174), Extract Class (182)
4. Chapter 8 — Move Function (198), Move Field (207)
5. Chapter 9 — Split Variable (240), Rename Field (244), Replace Derived Variable with Query (248)
6. Chapter 11 — Preserve Whole Object (319), Replace Parameter with Query (324), Remove Flag Argument (314)
