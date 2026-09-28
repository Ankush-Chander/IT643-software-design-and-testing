# The data model

## Quick recap
- Design principles, and the four ways a design rots
- The five principles, at the level of one class

---
## The claim

You have been taught to write classes that are *correct*. This lecture is about writing
classes that are ==usable==.

> A language already defines what it means to be a sequence, a number, something you can
> print, compare, or loop over. ==Your class can claim those meanings instead of
> inventing its own.==

---
### Agenda

- What the data model is
- Making an object readable: `__repr__` and `__str__`
- Making an object behave like a sequence
- Making an object behave like a number
- The same idea in C++, Java, Rust, Go
- What you take on when you claim a protocol

---
## What the data model is

The set of **protocols** the interpreter itself uses. A protocol is a name and a promise:
implement `__len__`, and `len()` works on your object.

Python's data model covers:

1. Objects, values and types
2. The standard type hierarchy
3. **Special methods** ← this lecture
4. Coroutines

---
#### The rule that confuses everyone first

==Special methods are called by the interpreter, not by you.==

```python
len(deck)        # you write this
deck.__len__()   # not this
```

You do not call `my_object.__repr__()`. You call `repr(my_object)`, and *Python* dispatches
to the method you wrote.

The dunder is the socket. The built-in function is the plug.

---
## Built-in types vs your types

| | |
|---|---|
| **numerics** | `int`, `float`, `complex` |
| **sequences** | `str`, `tuple`, `bytes`, `list`, `set`, `frozenset` |
| **mappings** | `dict` |
| **your types** | anything you write with `class` |

Built-in types get indexing, iteration, arithmetic and printing for free. ==The data model
is how a user-defined type asks for the same treatment.==

---
## 1. Make it readable

```python
class Lecture:
    def __init__(self, instructor, venue, topic):
        self.instructor, self.venue, self.topic = instructor, venue, topic

lec = Lecture("Ankush", "CEP-102", "The data model")
print(lec)
```

```
<__main__.Lecture object at 0x7d4a1ca31460>
```

A memory address. ==Every debugging session with this class starts by losing information.==

---
#### Two methods, two audiences

```python
class BetterLecture:
    def __init__(self, instructor, venue, topic):
        self.instructor, self.venue, self.topic = instructor, venue, topic

    def __repr__(self):
        return f"BetterLecture({self.instructor!r}, {self.venue!r}, {self.topic!r})"

    def __str__(self):
        return f"{self.topic}, by {self.instructor}"
```

```python
>>> print(lec)
The data model, by Ankush
>>> lec
BetterLecture('Ankush', 'CEP-102', 'The data model')
>>> [lec]
[BetterLecture('Ankush', 'CEP-102', 'The data model')]
```

Note `!r` in the repr. ==Without it the output is not valid Python==, which defeats the
point of a repr you could paste back.

| | Called by | For | Should read like |
|---|---|---|---|
| `__repr__` | `repr()`, the debugger, the REPL, a list's contents | **you** | code that could rebuild the object |
| `__str__` | `print()`, `str()`, f-strings | **the user** | a sentence |

If you write only one, write `__repr__` — `__str__` falls back to it, never the reverse.

---
## 2. Make it a sequence

What a list gives you for free:

```python
a = [1, 2, 4, 5, 6, 9]

a[0]                    # indexing
a[0:3]                  # slicing
for i in a: ...         # iteration
[i*i for i in a]        # comprehension
choice(a)               # random.choice
shuffle(a)              # random.shuffle
sorted(a)               # sorting
```

None of that is special to `list`. It is the sequence protocol.

---
#### The class that asks for none of it

```python
class NotGoodCardDeck:
    suits = 'spades diamonds clubs hearts'.split()
    ranks = [str(n) for n in range(2, 11)] + list('JQKA')

    def __init__(self):
        self._cards = [Card(rank, suit) for suit in self.suits for rank in self.ranks]

    def shuffle(self): ...
    def pick_random_card(self): ...
    def pick_cards_by_suit(self, suit): ...
```

Three methods you now have to write, name, document and test — and a user has to *learn*.
`shuffle` already exists. `pick_random_card` is `random.choice`.

==Every method here reinvents something the language already had a name for.==

---
#### Two methods instead

```python
Card = collections.namedtuple('Card', ['rank', 'suit'])

class CardDeck:
    suits = 'spades diamonds clubs hearts'.split()
    ranks = [str(n) for n in range(2, 11)] + list('JQKA')

    def __init__(self):
        self._cards = [Card(rank, suit) for suit in self.suits for rank in self.ranks]

    def __len__(self):
        return len(self._cards)

    def __getitem__(self, index):
        return self._cards[index]
```

---
#### What those two bought

```python
deck = CardDeck()

len(deck)                       # 52
deck[0]                         # Card(rank='2', suit='spades')
deck[0:3]                       # [Card('2','spades'), Card('3','spades'), Card('4','spades')]
Card('Q', 'hearts') in deck     # True
Card('Q', 'beasts') in deck     # False
next(iter(reversed(deck)))      # Card(rank='A', suit='hearts')
choice(deck)                    # Card(rank='9', suit='diamonds')
sorted(deck, key=spades_high)[-1]   # Card(rank='A', suit='spades')
```

Slicing, membership, iteration, `reversed`, `random.choice`, `sorted` — **none of which
were mentioned in the class.**

`__getitem__` alone makes an object iterable: Python falls back to calling it with
0, 1, 2 … until `IndexError`.

---
#### Reading is not writing

```python
shuffle(deck)
```

This needs a third method. `random.shuffle` works **in place**, so it has to assign:

```python
    def __setitem__(self, position, card):
        self._cards[position] = card
```

Without it:

```
TypeError: 'CardDeck' object does not support item assignment
```

==Protocols are graded.== Read-only sequence is one contract; mutable sequence is a
larger one. Claim only what you can honour.

---
## 3. Make it a number

```python
class Vector:
    def __init__(self, x, y):
        self.x, self.y = x, y

    def __str__(self):  return f"{self.x}i + {self.y}j"
    def __repr__(self): return f"Vector({self.x}, {self.y})"

    def __add__(self, other):
        return Vector(self.x + other.x, self.y + other.y)

    def __mul__(self, scalar):
        return Vector(scalar * self.x, scalar * self.y)

    def __rmul__(self, scalar):          # for `5 * v`, not `v * 5`
        return Vector(scalar * self.x, scalar * self.y)

    def __abs__(self):  return hypot(self.x, self.y)
    def __bool__(self): return bool(abs(self))
```

---
#### The result

```python
a, b = Vector(3, 4), Vector(5, 6)

print(a)            # 3i + 4j
repr(a)             # 'Vector(3, 4)'
a + b               # 8i + 10j
a * 5               # 15i + 20j
5 * a               # 15i + 20j
abs(a)              # 5.0
bool(a)             # True
bool(Vector(0, 0))  # False
```

Two details worth noticing:

- **`__rmul__` exists because `5 * a` asks `int` first.** `int` does not know about
  `Vector`, returns `NotImplemented`, and Python then asks the right operand. Without
  `__rmul__`, `a * 5` works and `5 * a` raises.
- **`__bool__` defined as "non-zero magnitude"** means `if vector:` reads as *"is this
  vector non-zero"* — a domain statement, written in language syntax.

---
## A library you already use

spaCy's `Doc` is a sequence of tokens, and nothing more exotic:

```python
doc = nlp("The iceberg is called the Python data model...")

doc[2]                                        # indexing  -> is
doc[5:8]                                      # slicing   -> Python data model
[t for t in doc if t.pos_ == "VERB"]          # comprehension
for token in reversed(doc): ...               # reversed
choice(doc)                                   # random.choice
```

*(Output as recorded in the source notebook.)*

There is no `doc.get_token_at(2)` and no `doc.get_slice(5, 8)`. ==The API is small because
the language already had names for most of what it does.==

---
## In your own code

Three of these are already in your repositories. ==22 of your 36 repos overload at least
one operator==, so this is not a foreign idea — it is one you use halfway.

---
#### Equality, written out four times

[`Param2725/snake_game`](https://github.com/Param2725/snake_game/blob/04e3db4/snake.cpp#L20)
declares the type and stops there:

```cpp
struct Position { int x, y; };          // no operator==
```

So every comparison is spelled out — `snake.cpp:244-249`:

```cpp
for (auto &o : obstacles) if (newHead.x == o.x && newHead.y == o.y) gameOver = true;
for (auto &s : snake)     if (newHead.x == s.x && newHead.y == s.y) gameOver = true;
if (newHead.x == food.x && newHead.y == food.y) { score++; ... }
```

Four coordinate comparisons in six lines. Add a `z` and every one of them is a bug.

---
#### One of you already fixed it

[`CharmiBhayani/SnakeCycle`](https://github.com/CharmiBhayani/SnakeCycle/blob/b9ea0e0/snakeCycle.cpp#L232-L237):

```cpp
struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Position& o) const { return !(*this == o); }
};
```

Four lines, written once. The three checks above become:

```cpp
newHead == o        newHead == s        newHead == food
```

`operator==` is C++'s `__eq__`. ==The comparison did not get shorter by accident — it got
shorter because the type now knows what equality means.==

---
#### Halfway there is still by hand

Same repo,
[`snakeCycle.cpp:326-333`](https://github.com/CharmiBhayani/SnakeCycle/blob/b9ea0e0/snakeCycle.cpp#L326-L333).
They defined `operator==` and **still** wrote the search:

```cpp
bool checkSelfCollision() const {
    if (body.size() <= 1) return false;
    Position head = body[0];
    for (size_t i = 1; i < body.size(); i++) {
        if (head == body[i]) return true;
    }
    return false;
}
```

The standard library already knows this loop:

```cpp
bool checkSelfCollision() const {
    return std::find(body.begin() + 1, body.end(), body.front()) != body.end();
}
```

Both agree on every input. ==Eight lines describing *how*, replaced by one line saying
*what*== — and it works only because `operator==` was already there.

In Python the same rule is `head in body[1:]`, via `__contains__`.

The identical loop appears in five repos: `dudhatmonar:65`, `Heer117:65`,
`LoveShah21:409`, `kkhushie:173`, `CharmiBhayani:326`.

---
#### A method that hides a method

Eight repos write some version of this:

```cpp
int    getLength() const { return body.size(); }     // CharmiBhayani:335, maahirgit:138
int    length()    const { return body.size(); }     // dudhatmonar:79
size_t getLength() const { return body.size(); }     // maitry4:245
```

The whole body is a call to the thing it is hiding, under a name every reader has to
learn — and a different name in each repo.

`size()` on the class, or `__len__` in Python, and `len(snake)` works for everyone with
nothing to learn.

---

!!! question "💬 Your snake game, as a sequence"

    The snake body is a list of coordinates. You wrote methods to grow it, check
    collisions, and draw it.

    Which dunder methods would let you delete code you already wrote?

    ??? hint "Answer"
        | Method | Deletes |
        |---|---|
        | `__len__` | `getLength()`, `size()`, `bodyCount()` |
        | `__getitem__` | `getSegment(i)`, `head()` becomes `body[0]` |
        | `__contains__` | the self-collision loop becomes `if head in body[1:]` |
        | `__iter__` | every `for i in range(len(body))` in the renderer |

        The self-collision check is the striking one. Most implementations write a loop
        comparing the head against each segment. With `__contains__`, the rule becomes one
        line that ==says what it means rather than how it is computed==.

        A caution: only claim `__getitem__` if indexing your object genuinely makes sense.
        A protocol you implement badly is worse than one you never claimed, because now
        every built-in that trusts it is wrong too.

---
## The same idea elsewhere

Python's version is unusually broad, but the idea is not Python's.

| Language | Mechanism | Examples |
|---|---|---|
| **C++** | operator overloading, iterators | `operator+`, `operator[]`, `operator<<`, `begin()`/`end()` |
| **Java** | interfaces | `toString`, `equals`/`hashCode`, `Comparable`, `Iterable`, `AutoCloseable` |
| **C#** | operators + interfaces | `operator+`, indexers `this[int i]`, `IEnumerable`, `IDisposable` |
| **Rust** | traits | `Display`, `Debug`, `Add`, `Index`, `Iterator`, `PartialOrd` |
| **Go** | implicit interfaces | `Stringer`, `sort.Interface`, `io.Reader`/`io.Writer` |

---
#### The same class, in C++

```cpp
class Vector {
public:
    Vector operator+(const Vector& o) const { return {x + o.x, y + o.y}; }
    friend Vector operator*(double s, const Vector& v) { return {s*v.x, s*v.y}; }
    friend std::ostream& operator<<(std::ostream& os, const Vector& v) {
        return os << v.x << "i + " << v.y << "j";
    }
private:
    double x, y;
};
```

`operator<<` is `__str__`. The free `operator*` taking the scalar first is `__rmul__`.
==Different spelling, identical idea: the type declares that it participates in an
existing vocabulary.==

Go states the principle most plainly — a type implements `Stringer` merely by having a
`String() string` method. No inheritance, no declaration.

---
## What you take on

Claiming a protocol is a promise to everything that already uses it.

| You implement | You promise |
|---|---|
| `__eq__` | equal objects stay equal, and hash the same |
| `__lt__` | a consistent ordering, or `sorted` misbehaves |
| `__len__` | a non-negative integer, cheap to compute |
| `__getitem__` | `IndexError` past the end, or iteration never stops |

==A built-in function does not verify your promise. It relies on it.==

That is the trade the whole lecture rests on: you get an enormous amount of existing code
for free, and in exchange you are held to the contract that code was written against.

---
## Closing

```python
import this
```

> *Beautiful is better than ugly. Simple is better than complex.*
> *Special cases aren't special enough to break the rules.*

Three questions to ask of any class you write from here:

1. Does it print as something a human can read?
2. Does it reinvent a verb the language already has?
3. If it behaves like a sequence or a number, does it **say so**?

---
## References:

1. Chapter 1, The Python Data Model — [*Fluent Python*](https://www.oreilly.com/library/view/fluent-python/9781491946237/ch01.html), Luciano Ramalho
2. [Python reference — Data model](https://docs.python.org/3/reference/datamodel.html#special-method-names)
3. [Difference between `__str__` and `__repr__`](https://stackoverflow.com/questions/1436703/difference-between-str-and-repr)
4. [spaCy v3: design concepts explained](https://youtu.be/BWhh3r6W-qE)
