# Test smells — the short list

For Part F. Read your own suite against these five and find two.

---

### Mystery guest

The test depends on something it does not show you — a file on disk, a seeded database, a
fixture built somewhere else.

**Tell:** you cannot tell what the input is by reading the test.
**Fix:** build the input in the test, or commit a small fixture next to it.

---

### Resource optimism

The test assumes an environment it did not create, and passes or vanishes depending on the
machine.

**Tell:** `if (file.is_open())` with no `else`. A skip on a data condition. A comment
saying "assuming there are at least three ___ in the database".
**Fix:** supply the resource, or move the test to a suite that is run separately and
knows it needs one.

---

### General fixture

One setup shared by every test, most of which no single test uses.

**Tell:** a `SetUp()` that resets globals. That is not organising the tests — it is
undoing the previous one.
**Fix:** take what the test needs as a parameter. A fixture that would disappear if the
design were better is telling you about the design.

---

### Eager test

One test function exercising several behaviours.

**Tell:** it is long, and when it fails you cannot tell which behaviour broke without
reading it.
**Fix:** the arrangement can stay shared. Split the *acts*.

---

### Assertion roulette

Several bare assertions with no message. It fails at line 42 and the report says: line 42.

**Tell:** a loop full of asserts, or a run of them on positional values.
**Fix:** name the thing, or assert on a whole structure at once so the diff shows you
everything that moved.

---

### And the one that is not on the list

A test that passes whatever the code does. `assert(result != nullptr)`. `EXPECT_GE(count, 0)`.
`if (results) { ... }`.

It is not a smell. It is a test that does not exist, written in a way that takes up space
in the report. If you find one of these, it counts for Part F and you should say so
plainly.
