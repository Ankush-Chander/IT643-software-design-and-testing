# Lab 4 — Group A__

> Copy this file to `lab4/REPORT.md`, fill it in, then paste the finished text into your
> pull request description. Keep the six headings exactly as they are — they are the
> marking scheme, in order. Delete this quote block and every `___` when you are done.

| | |
|---|---|
| Repository | ___ (your own snake game) |
| Base tag | `lab4-base` at commit ___ |
| Pull request | ___ |

---

## 1. Five rules — [5]

Written before opening the source. Behaviour, with an observable outcome.

| # | Rule |
|---|---|
| 1 | ___ |
| 2 | ___ |
| 3 | ___ |
| 4 | ___ |
| 5 | ___ |

If you could not state one of your own game's rules without going to look, say which and
why. It costs no marks.

___

---

## 2. What you could test, and what stopped you — [10]

No source changes in this part. Every `file:line` below is a line in `lab4-base`.

| # | Rule | Test written? | Blocking dependency (`file:line` + what it is) |
|---|---|---|---|
| 1 | ___ | ___ | ___ |
| 2 | ___ | ___ | ___ |
| 3 | ___ | ___ | ___ |
| 4 | ___ | ___ | ___ |
| 5 | ___ | ___ | ___ |

> **Rules testable without modifying the source: ___ / 5**

"It needs user input" is not a blocking dependency. `main.cpp:214 — getch() called inside
the game loop` is.

___

---

## 3. Coverage, and what it missed — [6]

| | |
|---|---|
| Line coverage | ___ % |
| Branch coverage | ___ % |
| Command used | ___ |

**One rule that is executed by the suite but not verified by it:**

| | |
|---|---|
| Rule | ___ |
| Line that runs | ___ |
| The assertion that is missing | ___ |

___

---

## 4. The seam — [10]

| | |
|---|---|
| Rule made testable | ___ |
| Commit 1 (seam) | ___ |
| Commit 2 (test) | ___ |
| Seam kind | object / link / preprocessing |
| Enabling point | ___ |
| What production code gave up | ___ |

The last row is graded. If the honest answer is "nothing", write that and say why the
seam cost nothing here.

___

---

## 5. The double — [4]

| | |
|---|---|
| What you passed through the seam | dummy / fake / stub / spy / mock |
| The method under test | ___ |

Two sentences: was the collaborator asked a question or told to do something, and why
does that decide the answer above?

___

---

## 6. Two smells in your own tests — [5]

| | Smell | `file:line` | One-line fix |
|---|---|---|---|
| 1 | ___ | ___ | ___ |
| 2 | ___ | ___ | ___ |

Fixing them is optional. Finding them is not.

___
