# Types as design

## Quick recap
- Architecture: boundaries, and the dependency rule

---

### Agenda

- Make illegal states unrepresentable
- Parse, don't validate
- Where errors should be caught

---
## Make illegal states unrepresentable

---
#### The starting code

```cpp
struct Snake {
    int  direction;     // 0=up 1=down 2=left 3=right
    bool alive;
    int  deathReason;   // 0=none 1=wall 2=self — meaningful only when !alive
    int  length;
    std::deque<Point> body;
};

void step(Snake& s) {
    Point h = s.body.front();
    if      (s.direction == 0) h.y--;
    else if (s.direction == 1) h.y++;
    else if (s.direction == 2) h.x--;
    else if (s.direction == 3) h.x++;
    s.body.push_front(h);
    s.body.pop_back();
}
```

**Which values can this struct hold that should be impossible?**

??? hint "Answer"
    | Field | Can hold | Should hold |
    |---|---|---|
    | `direction` | any `int` | 4 values |
    | `alive` + `deathReason` | dead with no reason, alive with a reason | 3 combinations |
    | `length` | anything | always `body.size()` |
    | `body` | empty | at least a head |

    Every bug on the opening slide was **legal**. The compiler checked the types, and the
    types allowed it.

---
#### Step 1 — `direction`

```cpp
enum class Direction { Up, Down, Left, Right };

struct Snake {
    Direction direction;    // was: int
    bool      alive;
    int       deathReason;
    int       length;
    std::deque<Point> body;
};
```

```cpp
s.direction = 5;
```

```
error: cannot convert 'int' to 'Direction' in assignment
```

`int` has about 4 billion values. `Direction` has 4.

---
#### Step 2 — `alive` and `deathReason`

```cpp
bool alive;
int  deathReason;   // 0=none 1=wall 2=self
```

**How many combinations of these two make sense?**

??? hint "Answer"
    Three: alive · dead from a wall · dead from itself.

    The pair can hold far more. *Dead, no reason* and *alive, hit a wall* are both legal.

---
#### Step 2 — one field instead of two

```cpp
enum class Status { Alive, HitWall, HitSelf };

struct Snake {
    Direction direction;
    Status    status;       // was: alive + deathReason
    int       length;
    std::deque<Point> body;
};
```

Three values. All meaningful.

==Two fields that must agree are one field.==

---
#### Step 3 — `length`

```cpp
int length;
std::deque<Point> body;
```

**What happens when these disagree?**

??? hint "Answer"
    They already did, on the opening slide: `length=4`, `body.size()=3`.

    Two places hold one fact. Every change must update both, and nothing makes it.

---
#### Step 3 — derive it

```cpp
class Snake {
public:
    std::size_t length() const { return body_.size(); }    // was: int length;

private:
    std::deque<Point> body_;
    Direction         direction_;
    Status            status_ = Status::Alive;
};
```

No field, nothing to disagree. `body_` is now private — only `Snake` can change it.

---
#### Step 4 — the empty snake

This still compiles:

```cpp
Snake empty;
```

**What does `empty.head()` return?**

??? hint "Answer"
    `head()` calls `body_.front()` on an empty `deque`. That is undefined behaviour — no
    error, no warning, anything can happen.

---
#### Step 4 — a constructor that needs a head

```cpp
class Snake {
public:
    explicit Snake(Point start, Direction d = Direction::Right)
        : body_{start}, direction_(d) {}

    Point head() const { return body_.front(); }   // body_ is never empty
    ...
};
```

```cpp
Snake empty;
```

```
error: no matching function for call to 'Snake::Snake()'
```

There is no way to make a snake without a head.

---
#### Step 5 — the dead snake that moves

```cpp
void step() {
    if (status_ != Status::Alive) return;   // the one new line
    Point h = head();
    switch (direction_) {
        case Direction::Up:    h.y--; break;
        case Direction::Down:  h.y++; break;
        case Direction::Left:  h.x--; break;
        case Direction::Right: h.x++; break;
    }
    body_.push_front(h);
    body_.pop_back();
}
```

```
alive, stepped: head=(6,5)
dead, stepped:  head=(6,5)
```

---
#### Try to break it now

| Attempt | Result |
|---|---|
| `s.direction_ = 5;` | does not compile |
| dead with no reason | no such value |
| `length` ≠ `body` | no `length` field |
| `Snake empty;` | does not compile |
| `s.body_.clear();` | `error: 'body_' is private` |
| dead snake steps | `step()` returns |

==An object that cannot be built in a bad state never has to be checked for one.==

??? note "The whole class"
    ```cpp
    enum class Direction { Up, Down, Left, Right };
    enum class Status    { Alive, HitWall, HitSelf };

    class Snake {
    public:
        explicit Snake(Point start, Direction d = Direction::Right)
            : body_{start}, direction_(d) {}

        Point       head()   const { return body_.front(); }
        std::size_t length() const { return body_.size(); }
        Status      status() const { return status_; }

        void turn(Direction d) { direction_ = d; }
        void die(Status why)   { status_ = why; }

        void step() {
            if (status_ != Status::Alive) return;
            Point h = head();
            switch (direction_) {
                case Direction::Up:    h.y--; break;
                case Direction::Down:  h.y++; break;
                case Direction::Left:  h.x--; break;
                case Direction::Right: h.x++; break;
            }
            body_.push_front(h);
            body_.pop_back();
        }
    private:
        std::deque<Point> body_;
        Direction         direction_;
        Status            status_ = Status::Alive;
    };
    ```

---
## Parse, don't validate

---
#### Reading a key

```cpp
bool isValidKey(char k) {
    return k == 'w' || k == 'a' || k == 's' || k == 'd';
}

void turnByKey(Snake& s, char k) {
    if      (k == 'w') s.turn(Direction::Up);
    else if (k == 's') s.turn(Direction::Down);
    else if (k == 'a') s.turn(Direction::Left);
    else if (k == 'd') s.turn(Direction::Right);
}

char k = getch();
if (isValidKey(k)) turnByKey(snake, k);
```

**What stops someone calling `turnByKey(snake, 'x')`?**

??? hint "Answer"
    Nothing.

    - `isValidKey` returns a `bool`. Once you branch on it, the fact *"this key is valid"*
      is gone
    - `turnByKey` still accepts any `char`. `'x'` falls through every branch, silently
    - The valid keys are written **twice**. Add arrow keys and you edit both
    - Nothing makes a caller run `isValidKey` first

---
#### `parse()` returns proof

```cpp
std::optional<Direction> parseKey(char k) {
    switch (k) {
        case 'w': return Direction::Up;
        case 's': return Direction::Down;
        case 'a': return Direction::Left;
        case 'd': return Direction::Right;
        default:  return std::nullopt;
    }
}
```

It does not answer *"is this valid?"*. It returns a `Direction` — or nothing.

---
#### Using it

```cpp
if (auto d = parseKey(getch())) snake.turn(*d);
```

```
w -> turned
x -> ignored at the boundary
d -> turned
```

`turn` takes a `Direction`. There is no way to write a call that hands it `'x'`.

| | validate | parse |
|---|---|---|
| returns | `bool` | `Direction`, or nothing |
| afterwards | the caller must remember | the type remembers |
| valid keys written in | two places | one |

---
#### Parse once, at the boundary

```
keyboard · files · config   →   parse   →   Direction · Score · GridSize
        untrusted                 edge           trusted, everywhere inside
```

Raw input becomes a trusted type at the edge. Nothing inside checks again.

---
#### A boundary type

```cpp
class GridSize {
public:
    static std::optional<GridSize> parse(int w, int h) {
        if (w < 10 || h < 10) return std::nullopt;
        return GridSize(w, h);
    }
    int width()  const { return w_; }
    int height() const { return h_; }
private:
    GridSize(int w, int h) : w_(w), h_(h) {}
    int w_, h_;
};
```

```
GridSize 20x15: ok
GridSize 4x4:   rejected
```

Build one without parsing:

```
error: 'GridSize::GridSize(int, int)' is private within this context
```

Every function that takes a `GridSize` knows it is at least 10 × 10.

---

!!! question "💬 Redesign this struct"

    A two-player game:

    ```cpp
    struct GameState {
        bool running;
        bool paused;
        bool over;
        int  winner;    // -1 = none, 0 = player 1, 1 = player 2
    };
    ```

    1. Which states can this hold that make no sense?
    2. Redesign it so they cannot be constructed.

    ??? hint "Answer"
        **The contradictions:**

        - `running` and `over` both true
        - `paused` and `over` both true
        - `running` and `paused` both true
        - a `winner` while the game is still running
        - `winner = -1` means two things: *not finished yet* and *draw*

        Three flags give 8 combinations. Three of them mean something.

        **One redesign:**

        ```cpp
        enum class Player { One, Two };

        struct Running {};
        struct Paused  {};
        struct Over    { std::optional<Player> winner; };   // nullopt = draw

        using GameState = std::variant<Running, Paused, Over>;
        ```

        ```
        running
        over: player 2 wins
        over: draw
        ```

        A winner can only exist inside `Over`. Try to give one to `Running`:

        ```
        error: too many initializers for 'Running'
        ```

        *Draw* is now spelled out, instead of hiding behind `-1`.

---
## The rule to keep

How did a dead snake move? **The struct allowed it.**

| | |
|---|---|
| If a state is wrong | make it unbuildable |
| If input is untrusted | parse it once, at the edge |

==Fail at the edge, not deep inside.==

The cost is more small types. Spend it where a bad state is a real bug — not on every
`int`.

---
## Practice — Python

Five before/after pairs, one per idea in this lecture.

For each: **what can go wrong?** Then rewrite it.

---

!!! question "💬 1 · Order status"

    ```python
    class Order:
        def __init__(self, status: str):
            self.status = status          # "pending", "paid" or "shipped"

    order = Order("pending")
    order.status = "shiped"

    if order.status == "shipped":
        notify_customer(order)
    ```

    ??? hint "Step 1 — What's wrong"
        ```
        order.status                -> 'shiped'
        order.status == "shipped"   -> False
        ```

        The typo is stored as if it were a real status. The `if` is never true, so the
        customer is never notified. No error anywhere.

    ??? hint "Step 2 — Fixed"
        ```python
        class Status(Enum):
            PENDING = "pending"
            PAID    = "paid"
            SHIPPED = "shipped"

        class Order:
            def __init__(self, status: Status):
                self.status = status

        order.status = Status.SHIPPED
        ```

        ```
        Status.SHIPED      -> AttributeError: type object 'Status' has no attribute 'SHIPED'
        Status("shiped")   -> ValueError: 'shiped' is not a valid Status
        Status("shipped")  -> <Status.SHIPPED: 'shipped'>
        ```

        **Magic values → enum.** `Status("shipped")` also parses a string from JSON or a
        database, so the enum works at the boundary too.

---

!!! question "💬 2 · Verified users"

    ```python
    @dataclass
    class User:
        email: str
        is_verified: bool = False
        verified_at: datetime | None = None
    ```

    ??? hint "Step 1 — What's wrong"
        ```
        User("a@b.com", is_verified=True)
          -> User(email='a@b.com', is_verified=True, verified_at=None)
        ```

        Verified — but never at any time. The reverse, unverified with a date, is legal
        too. Two fields hold one fact, and nothing keeps them in step.

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass
        class User:
            email: str
            verified_at: datetime | None = None

            @property
            def is_verified(self) -> bool:
                return self.verified_at is not None
        ```

        ```
        User("a@b.com").is_verified          -> False
        after setting verified_at            -> True
        User("a@b.com", is_verified=True)    -> TypeError: unexpected keyword argument 'is_verified'
        ```

        **Two fields that must agree are one field.** The other is derived.

---

!!! question "💬 3 · Hotel stays"

    ```python
    def book_stay(guest, check_in: date, check_out: date):
        if check_out <= check_in:
            raise ValueError("check-out must be after check-in")
        save(guest, check_in, check_out)

    def invoice(check_in: date, check_out: date, rate: int) -> int:
        return (check_out - check_in).days * rate
    ```

    ??? hint "Step 1 — What's wrong"
        ```
        invoice(Oct 5, Oct 2, 100)   -> -300
        ```

        The rule lives in `book_stay` only. `invoice` trusts its caller and produces a
        negative bill. Every new function that takes two dates has to remember the check.

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass(frozen=True)
        class Stay:
            check_in: date
            check_out: date

            def __post_init__(self):
                if self.check_out <= self.check_in:
                    raise ValueError("check-out must be after check-in")

            @property
            def nights(self) -> int:
                return (self.check_out - self.check_in).days

        def invoice(stay: Stay, rate: int) -> int:
            return stay.nights * rate
        ```

        ```
        Stay(Oct 5, Oct 2)            -> ValueError: check-out must be after check-in
        invoice(Stay(Oct 2, Oct 5))   -> 300
        stay.check_out = Oct 1        -> FrozenInstanceError: cannot assign to field 'check_out'
        ```

        **Invariant in the constructor.** `frozen=True` keeps it true afterwards.

---

!!! question "💬 4 · Email signup"

    ```python
    def is_valid_email(s: str) -> bool:
        return "@" in s and "." in s.split("@")[-1]

    def subscribe(raw: str):
        if is_valid_email(raw):
            send_welcome(raw)
            add_to_newsletter(raw)

    def send_welcome(email: str): ...
    def add_to_newsletter(email: str): ...
    ```

    ??? hint "Step 1 — What's wrong"
        - `send_welcome` and `add_to_newsletter` accept **any** `str` — nothing makes a
          caller run `is_valid_email` first
        - once the `if` passes, the fact *"this was checked"* is gone
        - no normalisation: `" Ada@X.com"` and `"ada@x.com"` become two subscribers

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass(frozen=True)
        class Email:
            value: str

            def __post_init__(self):
                local, _, domain = self.value.partition("@")
                if not local or "." not in domain or self.value != self.value.strip().lower():
                    raise ValueError(f"not a normalised email: {self.value!r}")

            @classmethod
            def parse(cls, raw: str) -> "Email":
                return cls(raw.strip().lower())

        def send_welcome(email: Email): ...
        def add_to_newsletter(email: Email): ...
        ```

        ```
        Email.parse(" Ada@Example.COM ")   -> Email(value='ada@example.com')
        Email.parse("ada.example.com")     -> ValueError: not a normalised email
        Email(" Ada@X.com")                -> ValueError: not a normalised email
        ```

        **Parse, don't validate.** Functions that take an `Email` never check again.

        *`NewType("Email", str)` is not enough: it labels the type for a type checker,
        but `Email("nope")` returns the plain string `'nope'` at runtime.*

---

!!! question "💬 5 · An order API"

    ```python
    def create_order(payload: dict) -> int:
        if payload.get("qty", 0) <= 0:
            raise ValueError("qty must be positive")
        total = payload["qty"] * payload["unit_price"]
        if payload.get("coupon"):
            total = apply_coupon(total, payload["coupon"])
        ship_to(payload["address"]["city"])
        return total
    ```

    ??? hint "Step 1 — What's wrong"
        ```
        {"qty": "3", ...}   -> TypeError: '<=' not supported between 'str' and 'int'
        no address          -> KeyError: 'address'      (after the total was computed)
        ```

        The raw `dict` travels into business logic. Every bad input fails in a different
        place, with a different error — some after work has already been done.

    ??? hint "Step 2 — Fixed"
        ```python
        @dataclass(frozen=True)
        class OrderRequest:
            qty: int
            unit_price: int
            city: str
            coupon: str | None = None

            def __post_init__(self):
                if self.qty <= 0:       raise ValueError("qty must be positive")
                if self.unit_price < 0: raise ValueError("unit_price cannot be negative")

        def parse_order(payload: dict) -> OrderRequest:
            try:
                return OrderRequest(
                    qty=int(payload["qty"]),
                    unit_price=int(payload["unit_price"]),
                    city=str(payload["address"]["city"]),
                    coupon=payload.get("coupon"),
                )
            except (KeyError, TypeError, ValueError) as e:
                raise ValueError(f"bad order: {e}") from e

        def create_order(order: OrderRequest) -> int:
            total = order.qty * order.unit_price
            if order.coupon:
                total = apply_coupon(total, order.coupon)
            ship_to(order.city)
            return total
        ```

        ```
        {"qty": "3", ...}   -> OrderRequest(qty=3, unit_price=100, city='Gandhinagar')
        no address          -> ValueError: bad order: 'address'
        qty 0               -> ValueError: bad order: qty must be positive
        ```

        **Parse once, at the boundary.** Inside, nothing takes a `dict`. Libraries like
        pydantic automate `parse_order`.

---
## References:

1. [Parse, don't validate](https://lexi-lambda.github.io/blog/2019/11/05/parse-don-t-validate/) — Alexis King
2. [Effective ML Revisited](https://blog.janestreet.com/effective-ml-revisited/) — Yaron Minsky (make illegal states unrepresentable)
3. [Principles](https://deviq.com/principles/) — DevIQ
