# Interfaces people can use

## Quick recap
- Simplicity: KISS, YAGNI, DRY, least power, information hiding

---
<!--## The question

```python
nums = [3, 1, 2]
nums = nums.sort()
print(nums)
```

**What does this print?**

??? hint "Answer"
    ```
    None
    ```

    The list was sorted, then thrown away. `nums` now holds what `sort()` returned.

    **Was that Python's mistake, or yours?** Hold your answer until the end.

----->

### Agenda

- Least astonishment — the call does what the caller expects
- Tell, don't ask — give the object the job, not the data
- Progressive disclosure — simple by default, more on request

---
## Least astonishment

A caller should be able to predict what a call does **from the call site alone**: its name,
what it returns, and what it changes.

If they have to open the function to find out, the interface has failed.

---
#### What the name promises

For each call: **does it change `a`? What does it return?** Decide before you open the answer.

```python
a = [3, 1, 2]

a.sort()
sorted(a)
a.reverse()
reversed(a)

s = "snake"
s.upper()
```

??? hint "Answer"
    | Call | Changes the original? | Returns |
    |---|---|---|
    | `a.sort()` | yes → `[1, 2, 3]` | `None` |
    | `sorted(a)` | no | `[1, 2, 3]` |
    | `a.reverse()` | yes → `[2, 1, 3]` | `None` |
    | `reversed(a)` | no | `<list_reverseiterator object>`, not a list |
    | `s.upper()` | no (strings never change) | `'SNAKE'` |

    The names are near-identical. The behaviour is not. You learn it by memory, not by
    reading.

---
#### A question that changes the answer

```cpp
class Snake {
public:
    bool isAlive() {
        step();                                   // move one cell
        return !hitsWall() && !hitsSelf();
    }
};

// renderer.cpp
if (snake.isAlive()) drawSnake(snake);

// hud.cpp
std::cout << (snake.isAlive() ? "ALIVE" : "DEAD");
```

**The snake should move one cell per frame. How far does it move?**

??? hint "Answer"
    **Two cells.** Every caller that asks moves it.

    ```
    head x at start: 5
    head x after one frame: 7
    ```

    Add a third caller, say a sound effect, and the snake speeds up. Reorder the callers,
    and the HUD shows a different answer from the renderer.

---
#### Command–query separation

A method either **does** something (a command) or **answers** something (a query). Not both.

> Asking a question should not change the answer. — Bertrand Meyer

```cpp
class Snake {
public:
    void step();                     // command: changes the snake, returns nothing
    bool isAlive() const;            // query:   const, cannot change the snake
};
```

**What does `const` buy you here?**

??? hint "Answer"
    The compiler enforces the split. A `const` query that tries to move the snake doesn't
    build:

    ```
    error: passing 'const Snake' as 'this' argument discards qualifiers
    ```

    ```
    head x after one frame: 6      # step() once, isAlive() twice
    ```

---
## Tell, don't ask

Don't pull data out of an object to make a decision **about that object**. Tell it what you
want, and let it decide.

On the next two slides:

| Role | Who |
|---|---|
| the **caller**, who asks or tells | `game.cpp` |
| the **object** being asked or told | the `Snake` |
| data the decision needs | the `Food` |

---
#### Asking

`game.cpp` **asks** the snake for its head and length, then decides how the snake grows:

```cpp
// game.cpp
if (snake.head() == food.pos()) {                 // ask the snake
    snake.setLength(snake.length() + 1);          // ask again, then decide for it
    score += 10;
    food.respawn();
}

// ai.cpp — plans ahead
if (snake.head() == food.pos()) {
    snake.setLength(snake.length() + 1);
}
```

New rule: **golden food makes the snake grow by 3.**

**How many places change? What can go wrong?**

??? hint "Answer"
    Every place that copied the rule. Each one reads the snake's data and decides for it.

    - Miss one, and the AI plans with the wrong length.
    - `setLength()` lets anyone set any length: `0`, `-1`, or a length with no body cells
      behind it.

    The rule about how a snake grows lives **outside** the snake, in every caller.

---
#### Telling

Each food knows what eating it is worth:

```cpp
enum class FoodKind { Normal, Golden };

class Food {
    Point    pos_;
    FoodKind kind_;
public:
    Point pos()    const { return pos_; }
    int   growth() const { return kind_ == FoodKind::Golden ? 3 : 1; }    // cells to grow
    int   points() const { return kind_ == FoodKind::Golden ? 30 : 10; }  // score to add
    void  respawn();                     // move to a new free cell
};
```

The snake decides whether it eats, and grows itself:

```cpp
class Snake {
public:
    bool eat(const Food& f) {            // grows if the head is on the food
        if (head() != f.pos()) return false;
        grow(f.growth());                // 1 for normal food, 3 for golden
        return true;
    }
private:
    void grow(int cells);                // no public setLength()
};

// game.cpp
if (snake.eat(food)) {                   // tell the snake: "eat this"
    score += food.points();
    food.respawn();
}
```

```
eat normal food:  length 1 → 2, score 0 → 10
eat golden food:  length 2 → 5, score 10 → 40
```

Who does what now:

- `game.cpp` **tells** the snake to eat. It no longer reads the snake's head or length.
- The snake **asks** the food for `growth()`. That's fine: it reads the food to decide
  about **itself**, not about the food.

The golden-food rule lives in `Food::growth()`. Changing it touches nothing else.

**Wait. `eat()` changes the snake *and* returns a `bool`. Doesn't that break
command–query separation?**

??? hint "Answer"
    **Yes, a little, on purpose.** The alternative is
    `if (snake.canEat(food)) snake.eat(food);`, which splits one rule into ask-then-tell
    again.

    The standard library makes the same trade: `std::set::insert` inserts *and* tells you
    whether it did.

    A command may report **what it just did**. It should never be a query in disguise, the
    way `isAlive()` was.

---
#### When asking is fine

**Which of these lines should become a "tell"?**

```cpp
renderer.draw(snake.cells());                                   // 1

if (snake.length() > 10) snake.setSpeed(snake.speed() + 1);     // 2

hud.show("Score: " + std::to_string(score.value()));            // 3
```

??? hint "Answer"
    **Only 2.** It reads the snake's state to change the snake's state: `snake.speedUp()`
    or a rule inside `step()`.

    Lines 1 and 3 ask in order to **display**. A renderer has to read positions. That isn't
    a decision about the snake.

    The smell is asking **in order to decide what to do to the object you asked**.

---
## Progressive disclosure

The common case needs **one simple call**. More control is there for whoever asks for it,
and invisible to everyone else.

---
#### One call, then a session

```python
r = requests.get("https://api.example.com/scores")
```

```python
s = requests.Session()
s.headers["Authorization"] = f"Bearer {token}"
s.mount("https://", HTTPAdapter(max_retries=3))
r = s.get("https://api.example.com/scores")
```

**How much did the first call need you to know about headers, retries or connection
pooling?**

??? hint "Answer"
    **Nothing.** It uses the same machinery as the second, with defaults.

    | Layer | Caller writes | Who needs it |
    |---|---|---|
    | 1 | `requests.get(url)` | almost everyone |
    | 2 | `get(url, timeout=5, headers=...)` | some |
    | 3 | `Session`, adapters, retries | a few |

    Each layer is optional. Nobody has to learn layer 3 to use layer 1.

---
#### Too much up front

```cpp
Game g(20, 20, 5, false, 3);
```

**What does `false` mean? What does `3` mean?**

??? hint "Answer"
    You can't tell without opening `Game`. And every caller must pass all five, even to
    get a normal game.

    Give each option a name and a default:

    ```cpp
    struct GameOptions {
        int  width     = 20;
        int  height    = 20;
        int  speed     = 5;
        bool wrapWalls = false;
        int  lives     = 3;
    };

    Game normal;                               // the common case: nothing to pass
    Game wrapped({.wrapWalls = true});         // C++20: name only what you change
    ```

    ```
    normal:  20x20 speed 5 wrap 0 lives 3
    wrapped: wrap 1 lives 3
    ```

---
## Critique these signatures

Three Python functions. For each: **which principle does it break, and how would you fix
it?**

---

!!! question "💬 1 · Coupon discount"

    ```python
    def get_discount(cart):
        if cart.coupon_used:
            return 0
        cart.coupon_used = True
        return round(cart.total * 0.1, 2)

    # cart page
    show(f"You save ₹{get_discount(cart)}")

    # checkout
    pay(cart.total - get_discount(cart))
    ```

    ??? hint "Step 1 — What's wrong"
        **Least astonishment.** `get_` reads like a query. It also uses up the coupon.

        ```
        cart page:  get_discount(cart)  -> 50.0
        checkout:   get_discount(cart)  -> 0        # the customer pays full price
        ```

    ??? hint "Step 2 — Fixed"
        ```python
        def discount(cart):                        # query: changes nothing
            return 0 if cart.coupon_used else round(cart.total * 0.1, 2)

        def apply_coupon(cart):                    # command: named for what it does
            d = discount(cart)
            cart.coupon_used = True
            return d
        ```

        ```
        discount(cart)      -> 50.0
        discount(cart)      -> 50.0
        apply_coupon(cart)  -> 50.0
        discount(cart)      -> 0
        ```

        **A query changes nothing. A command says so in its name.**

---

!!! question "💬 2 · Withdrawals"

    ```python
    # atm.py
    if account.balance >= amount and not account.frozen:
        account.balance -= amount

    # app.py
    if account.balance >= amount:
        account.balance -= amount
    ```

    ??? hint "Step 1 — What's wrong"
        **Tell, don't ask.** Each caller reads the account and decides for it. `app.py`
        forgot one rule.

        ```
        frozen account, balance 1000, withdraw 300 through app.py
        balance  -> 700
        ```

    ??? hint "Step 2 — Fixed"
        ```python
        class Account:
            def __init__(self, balance, frozen=False):
                self._balance = balance
                self._frozen = frozen

            def withdraw(self, amount):
                if self._frozen:
                    raise AccountFrozen("account is frozen")
                if amount > self._balance:
                    raise InsufficientFunds(f"balance {self._balance}, asked {amount}")
                self._balance -= amount

        # atm.py and app.py
        account.withdraw(amount)
        ```

        ```
        Account(1000, frozen=True).withdraw(300)  -> AccountFrozen: account is frozen
        Account(100).withdraw(300)                -> InsufficientFunds: balance 100, asked 300
        ```

        **The rule lives in one place: the object that owns the data.**

---

!!! question "💬 3 · Sending email"

    ```python
    def send_email(to, subject, body, cc, bcc, reply_to,
                   html, attachments, retries, timeout): ...

    send_email("a@x.com", "Hi", "Body", None, None, None, False, [], 3, 30)
    ```

    ??? hint "Step 1 — What's wrong"
        **Progressive disclosure.** The simplest email needs ten arguments, and seven of
        them are "nothing special".

        Which `None` is `bcc`? Swap two of them and nothing complains.

    ??? hint "Step 2 — Fixed"
        ```python
        def send_email(to, subject, body, *,
                       cc=(), bcc=(), reply_to=None, html=False,
                       attachments=(), retries=3, timeout=30): ...
        ```

        ```
        send_email("a@x.com", "Hi", "Body")                        # the common case
        send_email("a@x.com", "Hi", "<b>Body</b>", html=True)      # one option, by name
        send_email("a@x.com", "Hi", "Body", True)
            -> TypeError: send_email() takes 3 positional arguments but 4 were given
        ```

        **Required arguments by position, everything else by name with a default.** The `*`
        makes the rest keyword-only, so a bare `True` can't land in the wrong slot.

---
## The rule to keep

==Make the call site tell the truth.==

| Principle | Ask |
|---|---|
| Least astonishment | Can a caller predict what this does without opening it? |
| Tell, don't ask | Am I reading this object's data to decide what to do to it? |
| Progressive disclosure | Does the simplest use need only the simplest call? |

<!--Back to `nums = nums.sort()`. Python's FAQ explains the `None`:

> In order to remind you of that fact [that it sorts in place], it does not return the
> sorted list.

The surprise is deliberate. Every method that changes a list in place returns `None`, so
it can't be mistaken for one that makes a copy. **Command–query separation, built into the
language.**-->

---
## References:

1. *How to Design a Good API and Why it Matters* — Joshua Bloch (talk, 2006)
2. [TellDontAsk](https://martinfowler.com/bliki/TellDontAsk.html) — Martin Fowler
3. [CommandQuerySeparation](https://martinfowler.com/bliki/CommandQuerySeparation.html) — Martin Fowler
4. [Progressive Disclosure](https://www.nngroup.com/articles/progressive-disclosure/) — Nielsen Norman Group
5. [Why doesn't list.sort() return the sorted list?](https://docs.python.org/3/faq/design.html#why-doesn-t-list-sort-return-the-sorted-list) — Python Design FAQ
6. [Principles](https://deviq.com/principles/) — DevIQ
