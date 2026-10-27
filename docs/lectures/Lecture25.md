# Simplicity: KISS, YAGNI, DRY, least power, information hiding

## Quick recap
- Types as design: make illegal states unrepresentable, parse at the edge

---
<!--## The question

Two versions of a two-player snake game.

=== "A — copy-pasted"

    ```cpp
    void stepPlayer1(Snake& s) {
        Point h = s.next();
        if (h.x < 0 || h.x >= 20 || h.y < 0 || h.y >= 20) { s.die(); return; }
        s.moveTo(h);
    }

    void stepPlayer2(Snake& s) {
        Point h = s.next();
        if (h.x < 0 || h.x >= 20 || h.y < 0 || h.y >= 20) { s.die(); return; }
        s.moveTo(h);
    }
    ```

=== "B — a factory"

    ```cpp
    struct SnakeConfig {
        Point       start;
        int         speed     = 1;
        bool        wrapWalls = false;
        std::string skin      = "default";
    };

    class SnakeFactory {
    public:
        virtual ~SnakeFactory() = default;
        virtual std::unique_ptr<ISnake> create(const SnakeConfig& cfg) = 0;
    };

    class DefaultSnakeFactory : public SnakeFactory {
    public:
        std::unique_ptr<ISnake> create(const SnakeConfig& cfg) override {
            return std::make_unique<Snake>(cfg.start);   // speed, wrapWalls, skin: unused
        }
    };
    ```

**Which one is worse?** Hold your answer.

----->

### Agenda

- DRY — what duplication actually is
- YAGNI and KISS — what you build too early
- Least power — the weakest tool that does the job
- The tension between DRY and YAGNI, and a decision rule
- Information hiding — what to keep behind the boundary

---
## DRY — Don't Repeat Yourself

> Every piece of **knowledge** must have a single, unambiguous, authoritative
> representation within a system.
> — Hunt & Thomas, *The Pragmatic Programmer*

The word is *knowledge*, not *text*.

---
#### Same text, different knowledge

```cpp
// food.cpp
if (score % 5 == 0) spawnBonusFood();    // designer: a bonus every 5 points

// level.cpp
if (score % 5 == 0) speedUp();           // designer: faster every 5 points
```

Same line twice. Merge it?

```cpp
bool isMilestone(int score) { return score % 5 == 0; }
```

Next week: *"speed up every 10 points, keep bonuses at 5."*

**Which caller breaks when you edit `isMilestone`?**

??? hint "Answer"
    Both read it, so a change for one silently changes the other.

    The two lines look the same by coincidence. They are two rules that happen to share a
    number today. Leave them apart.

---
#### Duplicated knowledge

```cpp
// game.cpp
bool hitWall(Point p) { return p.x < 0 || p.x >= 20 || p.y < 0 || p.y >= 20; }

// food.cpp
Point randomFood() { return { rand() % 20, rand() % 20 }; }
```

The board becomes 30 × 20. Someone updates `hitWall`, misses `food.cpp`.

**What does the player see?**

??? hint "Answer"
    Food never appears in the new 10 columns. Update only `food.cpp` instead, and food
    appears outside the walls, where the snake can never reach it.

    One fact, *the board is 20 × 20*, lives in two places:

    ```cpp
    struct Board {
        int width, height;
        bool  contains(Point p) const { return p.x >= 0 && p.x < width && p.y >= 0 && p.y < height; }
        Point randomCell() const      { return { rand() % width, rand() % height }; }
    };
    ```

    The text in the two files was different. The knowledge was the same. That is the
    duplication DRY is about.

---
#### The rule of three

> The first time you do something, you just do it. The second time, you wince at the
> duplication, but you do it anyway. The third time, you refactor.
> — Don Roberts, quoted in Fowler's *Refactoring*

| Copy | What you know | Do |
|---|---|---|
| 1 | one example | write it |
| 2 | what *might* be shared | duplicate, note it |
| 3 | what is shared, what varies | extract |

Two examples cannot tell you which parts vary. Three usually can.

**Exception:** duplicated *knowledge* (the board size) gets fixed at copy two.
Its two copies already disagree the moment one changes.

---
## YAGNI and KISS

- **YAGNI** — *You Aren't Gonna Need It.* Build for today's requirement, not a guessed one.
- **KISS** — *Keep It Simple.* Of the designs that work, pick the one with fewest parts.

---
#### Speculative generality

```cpp
class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void drawSnake(const Snake&) = 0;
    virtual void drawFood(Point)         = 0;
};

class TerminalRenderer : public IRenderer { /* the only one */ };

class RendererFactory {
public:
    static std::unique_ptr<IRenderer> make(const std::string& kind) {
        if (kind == "terminal") return std::make_unique<TerminalRenderer>();
        throw std::invalid_argument(kind);
    }
};
```

Built because "we might add a GUI later." Fowler lists four costs of a feature built too
early:

| Cost | Here |
|---|---|
| **Build** | three classes instead of one |
| **Delay** | time not spent on what was asked |
| **Carry** | every reader jumps through three files to find what draws |
| **Repair** | when the GUI arrives, it needs `drawScore()` too, so the interface was wrong anyway |

When the GUI is real, extract the interface then. It takes ten minutes.

**Not speculative:** a second implementation that exists today, such as a fake renderer
in your tests.

---
#### What an agent adds that you didn't ask for

Prompt: *"Add a pause key to the snake game."*

=== "What the agent wrote (≈ 60 lines)"

    ```cpp
    struct GameConfig {
        bool enablePause  = true;
        char pauseKey     = 'p';
        int  pauseTimeout = 0;            // 0 = never
    };

    class IPauseStrategy {
    public:
        virtual ~IPauseStrategy() = default;
        virtual bool shouldPause(const InputEvent&) = 0;
    };

    class KeyPauseStrategy : public IPauseStrategy { /* ... */ };

    class KeyBindingManager {
        std::unordered_map<char, std::function<void()>> bindings_;
        /* ... */
    };
    ```

=== "What you needed (3 lines)"

    ```cpp
    case 'p': paused = !paused; break;    // in handleKey()

    if (paused) return;                   // first line of step()
    ```

Generating the code was free for you. Reading, reviewing and maintaining it is not.

**Review the diff for what you didn't ask for. Delete it.** Or say so in the prompt:
*"smallest change that works; no new classes or config."*

---
## The principle of least power

Use the **least expressive** mechanism that solves the problem.

> **Does this mechanism offer capabilities we do not actually need?**

Coined by Tim Berners-Lee for the web: HTML is data, not a program, which is why every
browser, search engine and screen reader can read it.

---
#### Three snake levels

Requirement: three levels. Each has its own **speed** and its own **obstacles**: blocked cells
inside the grid. The snake dies if it hits one, just as it does at the edge walls.

Level 2 on the 20 × 20 board: a line of obstacles along row 10, from column 0 to column 14.

```
         0         5         10      14        19
row  9   . . . . . . . . . . . . . . . . . . . .
row 10   # # # # # # # # # # # # # # # . . . . .
row 11   . . . . . . . . . . . . . . . . . . . .
```

Three ways to describe it:

=== "Constant"

    ```cpp
    constexpr int SPEED = 5;              // one speed, no obstacles, one level only
    ```

=== "Data"

    ```json
    { "speed": 5,
      "obstacles": [ { "row": 10, "from": 0, "to": 14 } ] }
    ```

=== "Code"

    ```cpp
    // levels.cpp: each level is a function the game calls at start
    void level2(Level& lv) {
        lv.speed = 5;
        for (int x = 0; x <= 14; ++x)
            lv.addObstacle({x, 10});
    }
    ```

The constant is too weak: it can't describe three levels. **Data or code?**

??? hint "Answer"
    **Data.** It is enough, and it is the weaker of the two.

    | Question | Data | Code |
    |---|---|---|
    | Fastest level? | read the files | run every level function |
    | Is this level valid? | check against a schema | run it, hope it halts |
    | What can a bad level do? | fail to parse | loop forever, write files, crash the game |
    | Who can edit it? | a level designer | a programmer, then a rebuild |

    Every capability the code has, and the data lacks, is something a reader or a tool
    can no longer check. Move up a rung only when the lower one can't express the need.

---
#### The ladder

| Rung | Example | Can be checked by reading? |
|---|---|---|
| Constant | `SPEED = 5` | yes |
| Data / config | `levels.json` | yes, against a schema |
| Small fixed language | regex, SQL `WHERE`, glob `*.cpp` | mostly |
| General code | a C++ level function, Python `eval`, plugins | only by running it |

Least power is YAGNI for capability: don't build in what you don't need, **and** don't
choose a tool that can do more than you need.

---
## The tension

| | Says |
|---|---|
| DRY | two copies, so abstract **now** |
| YAGNI | no proven need, so **wait** |

Both are right, about different things. The question is which mistake is cheaper to undo.

---
#### The wrong abstraction

Two callers draw a cell. Someone extracts:

```cpp
void drawCell(Point p, char glyph);
```

Four requirements later:

```cpp
void drawCell(Point p, char glyph, bool isHead, bool isFood,
              bool blink, int player) {
    if (isFood && blink && frame % 2) return;
    if (isHead)           setColour(player == 1 ? GREEN : BLUE);
    else if (isFood)      setColour(RED);
    else if (player == 2) setColour(CYAN);
    /* ... */
    putChar(p, glyph);
}
```

Each change was small. Nobody wanted to undo the extraction, so each one added a flag.

??? hint "The way out"
    Put the duplication back. Inline `drawCell` into each caller, delete the branches that
    caller never takes:

    ```cpp
    void drawHead(Point p, Colour c) { setColour(c); putChar(p, '@'); }
    void drawFood(Point p)           { if (blinkOff()) return; setColour(RED); putChar(p, '*'); }
    void drawBody(Point p, Colour c) { setColour(c); putChar(p, 'o'); }
    ```

    > Duplication is far cheaper than the wrong abstraction.
    > — Sandi Metz

---
#### A decision rule

Two pieces of code look alike. Ask, in order:

1. **Do they change for the same reason?** No → leave them apart. *(bonus vs speed-up)*
2. **Is it one fact written twice?** Yes → merge now. *(board size)*
3. **Is this the third copy?** No → duplicate and wait.
4. **Are you adding a flag to make it fit?** Yes → stop. Inline it back.

An interface, factory or config option needs a **second user today**, not a likely one.

---
## Information hiding

Hide each **design decision likely to change** behind a module boundary. Expose only what
clients need.

> **Can this implementation change without forcing changes throughout the system?**

David Parnas, 1972: split a system by *what is likely to change*, not by the steps of
the program.

---
#### A private field is not enough

```cpp
class Snake {
    std::deque<Point> body_;                        // private field
public:
    std::deque<Point>& body() { return body_; }     // getter: returns the field itself
};

// renderer.cpp
for (size_t i = 0; i < s.body().size(); ++i) draw(s.body()[i]);
// collision.cpp
for (auto& p : s.body()) if (p == h) die();
// ai.cpp
Point head = s.body().front();
```

`body_` is private, but the getter `body()` returns a **reference** to it. Every caller
gets the real `deque`: it can read it, index it, even `push_back` into it.

Collision checks are slow on long snakes. You want to add an `unordered_set<Point>` next to
the deque. **Which files change?**

??? hint "Answer"
    **All of them, and more.** Every caller knows the body is a `deque`, and indexes it. Any
    caller can also `push_back` into it and skip the set.

    Hide the decision. Expose what callers ask, not what you store:

    ```cpp
    class Snake {
        std::deque<Point>         body_;
        std::unordered_set<Point> cells_;    // added later: no caller notices
    public:
        Point head() const              { return body_.front(); }
        bool  occupies(Point p) const   { return cells_.count(p) > 0; }
        std::vector<Point> cells() const { return {body_.begin(), body_.end()}; }
    };
    ```

    The change now touches one file. Storage is a decision. `occupies()` is a question
    callers need answered.

---
#### What to hide in a snake game

| Decision | Likely to change? | Hide behind |
|---|---|---|
| how the body is stored | yes, for speed | `Snake` |
| board size | yes, per level | `Board` |
| high-score file format | yes, text → JSON | `ScoreStore` |
| that a snake has a head | no | nothing to hide |

Hide what is likely to change. Hiding what won't change is just a wrapper.

---
## Practice — Python

Six snippets. For each: **which principle does it break, if any?**

---

!!! question "💬 1 · Notifications"

    ```python
    class NotificationSender(ABC):
        @abstractmethod
        def send(self, to: str, body: str) -> None: ...

    class EmailSender(NotificationSender):
        def send(self, to, body):
            smtp.sendmail(FROM, to, body)

    class NotificationSenderFactory:
        @staticmethod
        def create(kind: str) -> NotificationSender:
            if kind == "email":
                return EmailSender()
            raise ValueError(kind)

    NotificationSenderFactory.create("email").send(user.email, msg)
    ```

    ??? hint "Step 1 — What's wrong"
        **Premature abstraction.** One kind of notification, three classes to reach it.

        - `create("emial")` fails at runtime, not when you write it.
        - To learn what `send` does, read the factory, then the ABC, then `EmailSender`.

    ??? hint "Step 2 — Fixed"
        ```python
        def send_email(to: str, body: str) -> None:
            smtp.sendmail(FROM, to, body)

        send_email(user.email, msg)
        ```

        **One implementation → a function.** When SMS is real, extract the interface
        then; both implementations will show you what it needs.

---

!!! question "💬 2 · GST"

    ```python
    def invoice_total(items):
        subtotal = sum(i.price * i.qty for i in items)
        return round(subtotal * 1.18, 2)

    def refund_amount(item, qty):
        return round(item.price * qty * 1.18, 2)
    ```

    ??? hint "Step 1 — What's wrong"
        **DRY violation.** One fact, *GST is 18%*, written twice.

        ```
        rate changes to 12%, only invoice_total updated
        invoice_total([pen x1 @ 100])  -> 112.0
        refund_amount(pen, 1)          -> 118.0     # refund exceeds what was paid
        ```

    ??? hint "Step 2 — Fixed"
        ```python
        GST_RATE = 0.18

        def with_gst(amount):
            return round(amount * (1 + GST_RATE), 2)

        def invoice_total(items):
            return with_gst(sum(i.price * i.qty for i in items))

        def refund_amount(item, qty):
            return with_gst(item.price * qty)
        ```

        ```
        GST_RATE = 0.12
        invoice_total([pen x1 @ 100])  -> 112.0
        refund_amount(pen, 1)          -> 112.0
        ```

        **Same fact → one place.** Fix it at the second copy; don't wait for a third.

---

!!! question "💬 3 · Length limits"

    ```python
    def validate_product(p):
        if len(p.name) > 50:
            raise ValueError("product name too long")

    def validate_address(a):
        if len(a.city) > 50:
            raise ValueError("city too long")
    ```

    ??? hint "Step 1 — What's wrong"
        **Nothing. This is fine.**

        The `50` for a product name is a catalogue decision. The `50` for a city comes from
        the courier's label format. They match by coincidence.

    ??? hint "Step 2 — Why it stays"
        The "DRY" version:

        ```python
        MAX_TEXT = 50

        def check_length(value, label):
            if len(value) > MAX_TEXT:
                raise ValueError(f"{label} too long")
        ```

        ```
        courier now allows 80-char cities; MAX_TEXT = 80
        validate_product(name of 70 chars)  -> passes     # catalogue page layout breaks
        ```

        **Same text, different reason to change → leave it.** At most, give each its own
        name: `MAX_PRODUCT_NAME = 50`, `MAX_CITY = 50`.

---

!!! question "💬 4 · Price formatting"

    ```python
    def format_price(amount, currency="INR", for_invoice=False,
                     for_email=False, round_up=False):
        if round_up:
            amount = math.ceil(amount)
        if for_invoice:
            return f"{currency} {amount:,.2f}"
        if for_email:
            return f"₹{amount:,.0f}" if currency == "INR" else f"{amount:,.0f} {currency}"
        return f"₹{amount:.2f}"
    ```

    ??? hint "Step 1 — What's wrong"
        **Premature abstraction, now the wrong one.** Every caller added a flag.

        ```
        format_price(99.5, for_invoice=True, for_email=True)  -> 'INR 99.50'   # email flag ignored
        format_price(99.5, currency="USD")                    -> '₹99.50'      # wrong symbol
        ```

        Three flags and a currency: dozens of combinations, a handful used. Nobody knows
        which of the rest are safe.

    ??? hint "Step 2 — Fixed"
        Inline it back into each caller, keeping only the branch that caller takes:

        ```python
        def invoice_price(amount, currency):
            return f"{currency} {amount:,.2f}"

        def email_price(amount):
            return f"₹{amount:,.0f}"

        def cart_price(amount):
            return f"₹{amount:.2f}"
        ```

        ```
        invoice_price(99.5, "INR")  -> 'INR 99.50'
        email_price(99.5)           -> '₹100'
        cart_price(99.5)            -> '₹99.50'
        ```

        **Growing flags → put the duplication back.** Three small functions, no
        impossible combinations.

---

!!! question "💬 5 · Discount rules"

    ```python
    # rules.txt — one rule per line, edited by the marketing team
    #   total > 1000 and user.is_member

    def discount_applies(rule: str, total, user) -> bool:
        return eval(rule)
    ```

    ??? hint "Step 1 — What's wrong"
        **Least power.** Every rule so far is a minimum total, sometimes members-only.
        `eval` accepts any Python.

        ```
        rule "total > 1000 and user.is_membr"
        discount_applies(...)  -> AttributeError: 'User' object has no attribute 'is_membr'
                                  # found at checkout, by a customer
        ```

        A rule can also read files, call the network, or loop forever. And nobody can answer
        "which rules are members-only?" without running them.

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass(frozen=True)
        class DiscountRule:
            min_total: int
            members_only: bool = False

            def applies(self, total, user) -> bool:
                return total > self.min_total and (user.is_member or not self.members_only)

        rules = [DiscountRule(**r) for r in json.load(open("rules.json"))]
        ```

        ```
        DiscountRule(**{"min_totl": 1000})
            -> TypeError: DiscountRule.__init__() got an unexpected keyword argument 'min_totl'
               # found when the file loads
        DiscountRule(min_total=1000, members_only=True).applies(1500, member)      -> True
        DiscountRule(min_total=1000, members_only=True).applies(1500, non_member)  -> False
        ```

        **Code → data.** Two fields cover every rule marketing has written. If they need
        more, add a field.

---

!!! question "💬 6 · Shopping cart"

    ```python
    class Cart:
        def __init__(self):
            self.items = {}               # sku -> qty

    # checkout.py
    total = sum(PRICES[sku] * qty for sku, qty in cart.items.items())

    # ui.py
    for sku, qty in cart.items.items():
        render_line(sku, qty)

    # promo.py
    if "FREE-MUG" not in cart.items:
        cart.items["FREE-MUG"] = 1
    ```

    New requirement: keep the price each item had **when it was added**.

    ??? hint "Step 1 — What's wrong"
        **Information hiding.** The decision *"a cart is a dict of sku → qty"* leaks into
        three files. Storing a price per line means changing all of them.

        Nothing guards the dict either:

        ```
        cart.items["PEN"]  = 2
        cart.items["BOOK"] = -1
        total  -> -180
        ```

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass(frozen=True)
        class Line:
            sku: str
            qty: int
            unit_price: int

        class Cart:
            def __init__(self):
                self._lines = {}

            def add(self, sku, qty, unit_price):
                if qty <= 0:
                    raise ValueError(f"qty must be positive, got {qty}")
                old = self._lines.get(sku)
                self._lines[sku] = Line(sku, qty + (old.qty if old else 0), unit_price)

            def quantity(self, sku):
                line = self._lines.get(sku)
                return line.qty if line else 0

            def lines(self):
                return tuple(self._lines.values())

            def total(self):
                return sum(l.qty * l.unit_price for l in self._lines.values())
        ```

        ```
        cart.add("PEN", 2, 10)
        cart.total()             -> 20
        cart.quantity("MUG")     -> 0
        cart.add("BOOK", -1, 200) -> ValueError: qty must be positive, got -1
        ```

        **Expose questions, hide storage.** The price snapshot changed only `Cart`.
        `checkout.py` calls `total()`, `ui.py` loops over `lines()`, and `promo.py` checks
        `quantity("FREE-MUG")`.

---
## The rule to keep

==Merge what changes together. Build only what is asked for today.==

| Principle | Ask |
|---|---|
| DRY | Is this one fact written in two places? |
| YAGNI | Is anyone using this today? |
| KISS | Is there a version with fewer parts that works? |
| Least power | Does this mechanism offer capabilities we do not actually need? |
| Information hiding | Can this implementation change without forcing changes throughout the system? |

<!--Back to the two snakes:

| | Problem | Fix |
|---|---|---|
| A — copy-pasted | the board size, written twice | `Board::contains()`, 3 lines |
| B — a factory | one kind of snake, three types, three unused config fields | delete it |

**B is worse.** A's problem takes 3 lines to fix. B gets harder to delete every time new
code calls the factory.-->

---
## References:

1. *The Pragmatic Programmer* — Hunt & Thomas (DRY)
2. *Refactoring*, 2nd ed. — Martin Fowler (the rule of three, speculative generality)
3. [The Wrong Abstraction](https://sandimetz.com/blog/2016/1/20/the-wrong-abstraction) — Sandi Metz
4. [Yagni](https://martinfowler.com/bliki/Yagni.html) — Martin Fowler
5. [Principles](https://deviq.com/principles/) — DevIQ
