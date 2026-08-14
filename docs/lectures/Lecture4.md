#### Quick recap
1. Deep vs shallow modules
---

#### Agenda
1. Refactoring
2. Code smells
3. Common refactoring techniques
---
#### Refactoring
> Refactoring (noun): a change made to the **internal structure of software** to make it easier to understand and cheaper to  modify **without changing its observable behavior**.

> Refactoring (verb): to restructure software by applying a series of refactorings without changing its observable behavior.

---

#### Why refactor?
- improves the design of software
- makes software easier to understand
- helps find bugs
- helps program faster
---
#### Code smells
> “If it stinks, change it.”  
> — Grandma Beck, discussing child-rearing philosophy  

- It"s easier to explain how to do refactoring than when.  
- No hard and fast rules  
- Comes with practice and intuition  

---
##### Long function
- **Small functions live longer** → easier to read, maintain, and reuse.  
- **Modern languages remove call overhead** → no excuse for long functions.  
- **Good naming reduces need to read body**.  
- **Rule of thumb**: When you feel like adding a comment → Extract a Function.  
- **Not about length** → about semantic distance between intent and implementation(what vs. how).

!!! question "💬 Refactor for lower semantic distance"
    ```cpp
    // says *how* — high semantic distance:
    for (int i = 0; i < users.size(); i++)
        if (users[i].age >= 18) eligible.push_back(users[i]);
    ```

    ??? hint "Refactored — says *what*"
        ```cpp
        vector<User> filter_adults(const vector<User>& users) {
            vector<User> eligible;
            for (const auto& user : users)
                if (user.age >= 18) eligible.push_back(user);
            return eligible;
        }

        auto eligible = filter_adults(users);
        ```
        - Give the intent a name (`filter_adults`) so the call site reads like English.
        - The reader no longer needs to understand the loop body to know *why* it exists.

- Long functions = Bad smell → aggressively decompose into meaningful, named functions.

---
##### Long function — in your code

!!! example "🔍 From your snake games"
    - [`krishadoshi16/Reptile-rush` — `Snake_game_final_code.cpp:13-310`](https://github.com/krishadoshi16/Reptile-rush/blob/021cd1d1bd271b6c5fd2e353d235026891fdc48d/Snake_game_final_code.cpp#L13-L310)
        - **`main()`, 298 lines** — window creation, snake construction, food
          spawning, bonus-food timing, obstacle placement, font loading, button
          layout, direction state, the event loop, collision detection, scoring, the
          render pass *and* the game-over screen, all in one un-named scope. The
          author has already marked **nine** extraction boundaries with
          `// ---------- Section ----------` banners. That is the rule of thumb
          above — *"when you feel like adding a comment → Extract a Function"* —
          firing nine times in one function.

    - [`sagar-dot-bera/ByteHebi` — `source/game.cpp:315-738`](https://github.com/sagar-dot-bera/ByteHebi/blob/b1702f0bc6a5f827dc5872bdd8c076de754478c9/source/game.cpp#L315-L738)
        - **`Game::render()`, 424 lines** — the longest function in the corpus.
          Board borders, gradient snake colouring, the HUD and three separate dialog
          modes in one body. Its mirror image, `processInput`, is another 205 lines.

    Neither is a smell *because* it is long. Both are a smell because the semantic
    distance between "draw one frame" and 424 lines of terminal calls is more than a
    reader can hold in their head.

    ??? tip "🔧 How would you fix these?"
        - **Reptile-rush** → **Extract Function** at each banner. The nine comments
          become nine names: `createWindow()`, `initSnake()`, `spawnFood()`,
          `placeObstacles()`, `layoutButtons()`, `drawFrame()`. `main()` shrinks to
          ~15 lines that read like a table of contents.
        - **ByteHebi** → **Split Phase** first: compute *what* the frame should show,
          then emit the terminal calls. Only then Extract Function per region
          (border, snake, HUD, each dialog).
        - **Order matters**: slide related statements together *before* extracting —
          you can only extract what is already adjacent.
        - **Stop when** each function fits one screen and its name says *what*, not *how*.

---
##### Duplicate code
- Duplication = more reading, harder changes, higher bug risk.
- Every copy must be checked → wasted effort.
- **Guiding principle:** Short, meaningful, unified functions = healthier codebase.
---
##### Duplicate code — in your code

!!! example "🔍 From your snake games"
    - [`JagratJani/snake-game-cpp` — `src/Food.cpp:18-82`](https://github.com/JagratJani/snake-game-cpp/blob/54d6b921a8abe16b4e12391ae539f26ea4ddbede/src/Food.cpp#L18-L82)
        - **The copy that already drifted.** The rejection-sampling loop that finds
          a free cell was copied from `GenerateWithObstacles` into
          `GenerateSpecialFood` — and the author *documented the copy in a comment*
          instead of removing it:

      ```cpp
      void Food::GenerateSpecialFood(const Snake& snake) {
          // Generate position first (same logic as regular food)
          bool invalidPosition;
          do {
              invalidPosition = false;
              x = rand() % maxX;
              y = rand() % maxY;
              ...
          } while (invalidPosition);
      ```

      The copy dropped the obstacle check, so **special food can spawn inside an
      obstacle and regular food cannot**. Nobody decided that; the duplicate
      drifted.

    - [`Piyushtanwani/Snake-game` — `main.cpp:357-543`](https://github.com/Piyushtanwani/Snake-game/blob/6068bd819da1eb985a262b1d979e30ff64be8497/main.cpp#L357-L543)
        - **The same four lines, seven times.** The head/body/food/empty glyph chain
          is written out seven times inside one `render()`, once per row that needed
          a different right-hand margin. Rendering logic was duplicated to
          accommodate a *layout* difference.

    Both dissolve the same way: **Extract Function** (`findFreeCell`, `cellGlyph`),
    then call it from every site.

    **A third kind — versioning by file copy.**
    [`202512057Meetsheth/Debug-Thugs`](https://github.com/202512057Meetsheth/Debug-Thugs/tree/4b2b3fa8197a8a0cfc1a5e30136540940d83cb9e)
    ships `part1.cpp` … `part5.cpp` plus `part52.cpp`: six standalone programs, each
    an evolved copy of the last (122 → 223 → 290 → 322 → 436 lines, and `part5.cpp`
    is headed `//Part 5 (final)`), with ~84 duplicated blocks between them. Three
    other repos do the same. **Branches and tags are how you keep versions — copying
    the file is how you lose track of which one is real.**

    ??? tip "🔧 How would you fix these?"
        - **snake-game-cpp** → Extract Function
          `findFreeCell(snake, obstacles)`; both generators call it. Pass obstacles
          as a **parameter**, so "special food ignores obstacles" has to be a
          decision someone types, not an accident of copying.
        - **Snake-game** → Extract `cellGlyph(x, y)`, then **one** loop over all
          rows, with the margin text pulled from a lookup table (`rowMargin[y]`).
          The layout difference stops duplicating the rendering logic.
        - **Do this first**: make the copies *identical* before extracting. If you
          extract over a drifted copy, you silently ship a behaviour change — here,
          special food would suddenly start respecting obstacles.
        - **Then decide** which behaviour was correct. Duplication hid the question;
          removing it forces the answer.

---
##### Divergent Change

- **Goal of structure**: Make change easy → one clear place to modify.
- **Smell**: When one module changes for _different reasons_ (e.g., database vs. financial logic).
- **Problem**: Mixed contexts → every change touches unrelated code → harder to understand & maintain.
- **Better design**: Separate contexts into distinct modules/classes.
---
##### Divergent Change — in your code

!!! example "🔍 From your snake games"
    - [`Dazzling-Darshan/SnakeByte` — `game.h:51-138`](https://github.com/Dazzling-Darshan/SnakeByte/blob/17bc256926a95aa647314649331fe78d4b52c41b/game.h#L51-L138)
        - **One class, five reasons to change** — and the history is annotated in
          the source. Every wave of requirements left a `// New:` marker behind:

      ```cpp
      pair<int,int> poisonFood;              // New: Poison food
      pair<int,int> shield;                  // New: Shield power-up
      bool paused;                           // New: Pause state
      const int SHIELD_SPAWN_INTERVAL = 45;  // Changed: ...to 60 seconds
      ```

        `class Game` changes for   
            - new collectibles,   
            - for rendering,   
            - for high-score,  
            - for file I/O,  
            - for pause semantics, and for difficulty tuning.   
        Adding one collectible means touching the member list, three declarations, spawn,
        update, the draw chain **and** the pause-timer compensation. The
        `// Changed: ...to 60 seconds` comment sitting above a literal `45` is
        itself evidence of how hard the class is to keep coherent.

    - [`GarvModi18/SnakeGameProject` — `snake.cpp:1-1028`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L1-L1028)
        - **One file, six concerns, 1028 lines** — ANSI console plumbing, Windows
          audio, high-score persistence, six UI screens, rendering and the game
          rules, all over shared mutable globals. Changing the audio backend, the
          save-file format or the collision rules all land in the same file.

    Ask of any module: *what kinds of change bring me here?* More than one answer is
    the smell.

    ??? tip "🔧 How would you fix these?"
        - **Split by *reason to change*, not by size.** A 1000-line file with one
          reason is fine; a 100-line class with five is not.
        - **SnakeByte** → a `Collectible` interface with one implementation per
          pickup (food, poison, shield, special) + a `HighScoreStore` for the file
          I/O + a `Clock` that owns pause compensation. Adding a pickup becomes
          *one new class*, not seven edits.
        - **SnakeGameProject** → split `snake.cpp` into `console`, `audio`,
          `highscore`, `ui`, `render`, `rules`. Replace the shared mutable globals
          with a `GameState` passed explicitly — otherwise the split is cosmetic
          and every module still reaches into everything.
        - **Test you did it right**: adding a new power-up should touch one file.

---
##### Shotgun Surgery

- **Smell**: Opposite of _Divergent Change_.
- Every change → many small edits across multiple classes/modules.
- Hard to track, easy to miss updates.
---
##### Shotgun Surgery — in your code

!!! example "🔍 From your snake games"
    - `GarvModi18/SnakeGameProject` — `snake.cpp`
      [`431-440`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L431-L440) ·
      [`527-536`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L527-L536) ·
      [`596-604`](https://github.com/GarvModi18/SnakeGameProject/blob/6770cf0f123401c1ae5fa4bdaca204455acdf52f/snake.cpp#L596-L604)
        - **One new menu key, three edits.** The arrow-key block — `kbhit()`,
          `getch()`, the `72`/`80` scancode `switch` — is repeated verbatim in
          `ShowSettingsPage()` (388), `ShowMenu()` (469) and `ShowPauseOverlay()`
          (554). All three are live in the same running program, so binding one new
          key means finding and correctly editing all three.

    - `Neel1585/Snake_Game` —
      [`level.cpp:10-31`](https://github.com/Neel1585/Snake_Game/blob/ac5dff594149f378b91a1b5a87d8d669ace3c13a/level.cpp#L10-L31) ·
      [`obstacle.cpp:9-29`](https://github.com/Neel1585/Snake_Game/blob/ac5dff594149f378b91a1b5a87d8d669ace3c13a/obstacle.cpp#L9-L29)
        - **Non-blocking input, copied per level.** `getchNonBlocking()` — 20 lines
          of `termios` plus a `select()` — is defined identically in both levels the
          README ships (`g++ level.cpp -o level`, `g++ obstacle.cpp -o obstacle`).
          Fixing terminal restore on Ctrl-C is two edits, and the copies have already
          diverged cosmetically, which defeats diffing.

    `snake.cpp` appeared under **Divergent Change** as well: it changes for six
    unrelated reasons *and* one menu change takes three edits inside it. The two
    smells are opposite ends of one defect — **the duplication is why the change is
    scattered.**

    ??? tip "🔧 How would you fix these?"
        - **SnakeGameProject** → Extract Function `readMenuKey()` returning an enum
          (`Up`, `Down`, `Select`, `Back`); all three screens call it. A new binding
          becomes one edit in one place.
        - **Snake_Game** → move `getchNonBlocking()` into `input.h` and include it
          from both levels. Better still: the two levels are ~90% the same file —
          make them one program with a level parameter.
        - **General recipe**: Move Function / Move Field to pull the scattered
          pieces into one module, then **Inline** whatever is left over.
        - **Don't over-correct.** Sweeping everything into one "utils" module just
          trades this smell for **Divergent Change**. Group by *what changes
          together*, not by *what is left over*.

---
##### Comments
- **Comments are good** → but often used as _deodorant_ to hide bad code.
- Heavy comments usually signal underlying _bad smells_.
- **First step**: Refactor → often eliminates the need for comments.
**When comments help**
- To explain _why_ something is done (not what).
- To flag uncertainty or future concerns.
**Principle:** _Write clean code that explains itself — use comments for intent, not explanation._
---
##### Comments — in your code

!!! example "🔍 From your snake games"
    - [`mshaikh19/Snake-Game-Project` — `GameGrid.cpp:314-349`](https://github.com/mshaikh19/Snake-Game-Project/blob/b3ecddd6c5668bb0c876fb6d364b3cf47a1e7d33/GameGrid.cpp#L314-L349)
        - **Commented-out code used as version control.** An entire previous version
          of the HUD banner — 36 lines, same box-drawing art, one column wider —
          sits commented out directly below the live version. Four more dead blocks
          live in the same file. `drawGameGrid` is 166 lines, **45 of them
          comments**, most of that a commented-out duplicate of the code above it.
          The repo is under git; the history already holds the old version.

    - [`krishadoshi16/Reptile-rush` — `Snake_game_final_code.cpp:24-234`](https://github.com/krishadoshi16/Reptile-rush/blob/021cd1d1bd271b6c5fd2e353d235026891fdc48d/Snake_game_final_code.cpp#L24-L234)
        - **Comments standing in for function names.** Nine
          `// ---------- Section ----------` banners inside a single 298-line
          `main` — each one a function name that was never written:
          `createWindow()`, `initSnake()`, `spawnFood()`, `placeObstacles()`,
          `layoutButtons()`, `drawFrame()`. Refactor first, and the comments delete
          themselves.

    **The contrasting good case**, from `Dazzling-Darshan/SnakeByte`
    [`game.h:10`](https://github.com/Dazzling-Darshan/SnakeByte/blob/17bc256926a95aa647314649331fe78d4b52c41b/game.h#L10) —
    `#include <fstream> // Required for file I/O (High Score)`. That one explains
    **why**. Compare with `// New: Poison food` a few lines below it: six months on,
    everything is "new".

    ??? tip "🔧 How would you fix these?"
        - **Delete commented-out code.** `git log` *is* your version control. Dead
          code costs you a re-read every time and is never the version you want back.
        - **Banner comments → function names** (Extract Function). The comment
          becomes the signature and stops being able to go stale.
        - **`// New: X` → nothing.** `git blame` already answers "when". If the
          comment tracks a diff rather than an intent, it expires.
        - **Keep** comments that answer *why*: a non-obvious constraint, a workaround
          and its cause, a warning about what looks safe but isn't.
        - **Rule of thumb**: a comment describing *what* the next lines do is a
          function waiting to be extracted.

---
#### Common refactoring techniques
---
##### Extract function

![](../images/refactoring/extract_function.png)

!!! question "💬 Can you extract a function from this cluttered body?"
    ```javascript
    function printOwing(invoice) {
        printBanner();
        let outstanding = calculateOutstanding();
        //print details
        console.log(`name: ${invoice.customer}`);
        console.log(`amount: ${outstanding}`);
    }
    ```

    ??? hint "After Extract Function"
        ```javascript
        function printOwing(invoice) {
            function printDetails(outstanding) {
                console.log(`name: ${invoice.customer}`);
                console.log(`amount: ${outstanding}`);
            }
            printBanner();
            let outstanding = calculateOutstanding();
            printDetails(outstanding);
        }
        ```
        - The "print details" block got its own named function → the comment is no longer needed.

---
##### Inline function 
!!! question "💬 Is this helper function adding clarity — or indirection?"
    ```javascript
    function moreThanFiveLateDeliveries(driver) {
        return driver.numberOfLateDeliveries > 5;
    }

    function getRating(driver) {
        return moreThanFiveLateDeliveries(driver) ? 2 : 1;
    }
    ```

    ??? hint "After Inline Function"
        ```javascript
        function getRating(driver) {
            return (driver.numberOfLateDeliveries > 5) ? 2 : 1;
        }
        ```
        - The body was just as clear as the name → inlining removes an unnecessary layer of indirection.
---
- Inverse of `Extract Function`

- **When to Use**
	- Function’s body = just as clear as its name.
	- Too much delegation → hard to trace logic.
	- Want to regroup/refactor functions.

- **Mechanics**
	1. Ensure it’s **not polymorphic** (no subclass overrides).
	2. Find all callers.
	3. Replace call with body → test after each.
	4. Remove function definition.
	5. Inline gradually if tricky (multiple returns, recursion).
- **Principle:** _Indirection is good only when it adds clarity — inline when it doesn’t._

---
##### Slide statements
!!! question "💬 Are related statements grouped together?"
    ```javascript
    const pricingPlan = retrievePricingPlan();
    const order = retreiveOrder();
    let charge;
    const chargePerUnit = pricingPlan.unit;
    ```

    ??? hint "After Slide Statements"
        ```javascript
        const pricingPlan = retrievePricingPlan();
        const chargePerUnit = pricingPlan.unit;
        const order = retreiveOrder();
        let charge;
        ```
        - `pricingPlan` and its consumer (`chargePerUnit`) are now adjacent → easier to extract into a function later.
---
- **Problem**
	- Related code is **scattered**, mixed with unrelated logic.
	- Harder to understand, modify, or extract into functions.
- **Solution**
	- **Move related statements together** so intent is clearer.
	- Often a preparatory step for **Extract Function**.
 **Principle:** _Group related logic together → clarity first, then refactor further._

---
##### Replace temp with query

!!! question "💬 Can you eliminate the temp variable?"
    ```javascript
    const basePrice = this._quantity * this._itemPrice;
    return basePrice > 1000 ? basePrice * 0.95 : basePrice * 0.98;
    ```

    ??? hint "After Replace Temp with Query"
        ```javascript
        getBasePrice() {
            return this._quantity * this._itemPrice;
        }

        // call function instead of temp variable
        return this.getBasePrice() > 1000 ? this.getBasePrice() * 0.95 : this.basePrice * 0.98;
        ```
        - The temp becomes a reusable query → easier to extract other functions without passing the temp around.
---

**Motivation**
- Easier function extraction (no temps to pass around).
- Stronger boundaries → fewer dependencies & side effects.
- Eliminates duplicate calculation logic.
- Best inside a class (shared context for queries).
---
##### Split Loop
!!! question "💬 This loop does too many things at once — how do you split it?"
    ```javascript
    let averageAge = 0;
    let totalSalary = 0;
    for (const p of people) {
        averageAge += p.age;
        totalSalary += p.salary;
    }
    averageAge = averageAge / people.length;
    ```

    ??? hint "After Split Loop"
        ```javascript
        let totalSalary = 0;
        for (const p of people) {
            totalSalary += p.salary;
        }

        let averageAge = 0;
        for (const p of people) {
            averageAge += p.age;
        }
        averageAge = averageAge / people.length;
        ```
        - Each loop now has a single responsibility → ready for **Extract Function** next.

---
**Problem**  
- One loop doing **multiple things at once**.  
- Makes modifications harder → must understand _all_ behaviors.  
- Leads to cluttered code with multiple outputs/side effects.  

**Solution**  
- **Split loop into separate loops**, each doing one task.  
- Improves clarity and maintainability.   
- Often followed by **Extract Function** on each loop.  

---
##### Split phase 
!!! question "💬 This code mixes parsing and pricing — can you separate the concerns?"
    ```javascript
    const orderData = orderString.split(/\s+/);
    const productPrice = priceList[orderData[0].split("-")[1]];
    const orderPrice = parseInt(orderData[1]) * productPrice;
    ```

    ??? hint "After Split Phase"
        ```javascript
        function parseOrder(aString) {
            const values = aString.split(/\s+/);
            return ({productID: values[0].split("-")[1], quantity: parseInt(values[1])});
        }
        function price(order, priceList) {
            return order.quantity * priceList[order.productID];
        }

        const orderRecord = parseOrder(order);
        const orderPrice = price(orderRecord, priceList);
        ```
        - Parsing and pricing are now separate phases → each is independently testable.
---

**Clues**  
- Different parts use different data/functions.  
- Sequential steps doing _significantly different_ work.  

**Mechanics**  
1. Extract second phase into its own function.  
2. Add intermediate data structure (passed between phases).  
3. Move relevant parameters/fields into the structure.  
4. Extract first phase separately.  


**Principle:** _Separate concerns into phases → easier to reason about, test, and extend._

---
#### References

1. Chapter 1, [Refactoring, Second edition](https://martinfowler.com/books/refactoring.html) by Martin Fowler and Kent Beck
2. [Refactoring example in Javascript](https://github.com/vitorfranca/Refactoring-Martin_Fowler/tree/main/Chapter-1)

!!! note "About the examples"
    Every "in your code" link above is pinned to a **commit SHA**, not a branch, so
    it keeps pointing at the code as it was when this lecture was written — your
    later commits will not shift the line numbers out from under it. All examples
    were read and verified against the source, not inferred from metrics.

    Nothing here is a mark against the author. Every one of these appears in code
    that *works*; they are the shapes that make the **next** change expensive, which
    is exactly what refactoring is for.
