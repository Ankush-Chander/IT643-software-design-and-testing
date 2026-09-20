## **Lab 4: The tests you cannot write**

**Repo: your own snake game.** Not the one you reviewed. Not the one you changed in Lab 1.

**Deadline: Saturday(26/Sep/2026), 11:59 PM.**

**Submit:** one PR on your own repo, two commits, report in the PR description.

**Time: ~4 hours.** Part B is half of it.

---

#### **Step 0 — Pin the base (2 min)**

```bash
git checkout main            # or your default branch
git pull
git tag lab4-base
git push origin lab4-base
git checkout -b lab4

mkdir -p lab4
curl -L -o lab4/REPORT.md \
  https://raw.githubusercontent.com/Ankush-Chander/IT643-software-design-and-testing/main/docs/labs/lab4/REPORT-template.md
```

Every `file:line` you cite in Part B must exist in `lab4-base`. That is how this lab is
checked.

---

#### **Part A – Five rules (15 min) — [5]**

Write five things your game does, in plain English. **Before opening the source.**

Good:

> The snake dies when its head enters its own body.
> Eating fruit increases the score by exactly one.
> Fruit never appears on a cell the snake occupies.

Bad: *"the board is drawn with `#`"* — that is rendering, not behaviour. You cannot
assert on it.

→ Section 1 of the report.

---

#### **Part B – Try to test all five (90 min) — [10]**

**Rule: you may not change one line of source in this part.**

Allowed: new test files, a build target, linking gtest.
Not allowed: touching the game.

For each rule, either write the test or record what stopped you.

| Rule | Test written? | Blocking dependency (`file:line` in `lab4-base`) |
|---|---|---|

Then the number this lab is about:

> **How many of five could you test without changing the source?**

Expect one or two. Across your 36 repos: `getch` in 32, `rand` in 30, `sleep` in all 36.

**A low number costs you nothing.** Five out of five, propped up by tests that assert
nothing, costs you the section.

Name the blocker precisely:

- ❌ "it needs user input"
- ✅ `main.cpp:214 — getch() called inside the game loop, no way to supply keys`

→ Section 2.

---

#### **Part C – Coverage (30 min) — [6]**

```bash
g++ --coverage -O0 -g your_game.cpp tests.cpp -o tests
./tests
gcov -b your_game.cpp
```

Record line % and branch %.

Then answer one question:

> **Which of your five rules runs during a test but is never checked by one?**

Every suite has one. Name the rule, the line, and the assertion that is missing.

→ Section 3.

---

#### **Part D – One seam, one PR (60 min) — [10]**

Take the rule that was hardest in Part B. Make it testable.

**Two commits, this order:**

| # | Contains | Must not contain |
|---|---|---|
| 1 | the seam | any behaviour change |
| 2 | the test | any source change |

Check commit 1: run the game. If a player can see any difference, it is the wrong commit.

Then open the PR, `lab4` → `main`, titled:

```
Lab 4: <the rule you made testable>
```

Report three things:

| | |
|---|---|
| Seam kind | object / link / preprocessing |
| Enabling point | where the decision now lives |
| What production code gave up | ___ |

The third row is graded. "Nothing" is a valid answer if you say why.

→ Section 4.

---

#### **Part E – Name the double (10 min) — [4]**

Whatever you passed through the seam: dummy, fake, stub, spy, or mock.

Two sentences on why, answering one question:

> Was the collaborator **asked a question**, or **told to do something**?

A method that returns a value can be checked by its return value. A method that returns
nothing cannot — the only thing left to assert is that the call happened.

→ Section 5.

---

#### **Part F – Two smells in your own tests (20 min) — [5]**

Read your new suite against [`lab4/SMELLS.md`](lab4/SMELLS.md).

Find **two**. Report `file:line`, the smell, the one-line fix. Fixing is optional.

"No smells found", in a suite written this week under a deadline, is not credible.
Nobody loses marks for what they find.

→ Section 6.

---

#### **Checklist before you submit**

- [ ] `lab4-base` tag pushed
- [ ] PR open, `lab4` → `main`, on **your own** repo
- [ ] Exactly **two** commits
- [ ] Commit 2 changes no source file
- [ ] Report pasted into the PR description
- [ ] Every Part B `file:line` exists in `lab4-base`

---

#### **Marks**

| Part | | Marks |
|---|---|---|
| A | Five rules | 5 |
| B | The count, and the blockers | 10 |
| C | Coverage vs verification | 6 |
| D | The seam, in two commits | 10 |
| E | The double, justified | 4 |
| F | Two smells in your own tests | 5 |
| | **Total** | **40** |
