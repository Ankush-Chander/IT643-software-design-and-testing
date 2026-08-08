
## **Lab Assignment 3: Make it multiplayer — in someone else's code**

#### **Problem Statement**

In Lecture 3 you read another group's snake game and predicted which functions
would have to change to support a second player. Now you make that change.

You are modifying **the repository you reviewed**, not your own.

That is deliberate. A design is easy to change if a *stranger* can change it. You
are the stranger. What it costs you is the measurement.

**Deadline: Wednesday, 12:00 PM.**

---

#### **Part A – The change**

Implement exactly this, and nothing more:

1. A **second snake**, controlled by `W` `A` `S` `D`. Player 1 keeps whatever
   controls the game already uses.
2. Both snakes eat the **same fruit**. Whoever reaches it first grows.
3. Each player has their **own score**, both visible during play.
4. The game ends when **either** snake hits a wall, itself, or the other snake.
   Announce which player lost.

**Do not do anything else.** No new menus, no colours, no renaming, no tidying up
code you dislike, no reformatting. Every extra edit makes your measurement
incomparable to the other 41 groups, and the comparison is the assignment.

If you think the code needs restructuring before a second snake is possible, that
is a finding — write it in Part D. Do not act on it.

---

#### **Part B – Getting the code and opening the PR**

You do not have write access, so you work from a fork.

```bash
# 1. Fork the repo on GitHub (the "Fork" button), then:
git clone https://github.com/<your-username>/<repo-name>.git
cd <repo-name>
git checkout -b feat/multiplayer

# 2. Before you touch anything, protect the diff from line-ending noise:
git config core.autocrlf false

# 3. ... make the change ...

git commit -am "Add second player"
git push -u origin feat/multiplayer
```

Then open a **Pull Request from your fork to the original repository**.

**Before you push, sanity-check your diff:**

```bash
git diff --stat
```

If this says you changed 800 lines in a file you barely touched, your editor
reformatted it or rewrote the line endings. Fix that before pushing — a diff full
of whitespace noise cannot be measured and will be sent back.

**If more than one of you is writing code**, do not open separate PRs. One member
forks and creates `feat/multiplayer`; everyone else branches off *that* and merges
back into it:

```bash
git checkout feat/multiplayer
git checkout -b feat/multiplayer-scoring   # your slice of the work
# ... work, commit, push ...
```

Open your sub-branch PR **into `feat/multiplayer` on the fork**, not into the
original repo. Your group submits exactly one PR upstream, from `feat/multiplayer`.
That keeps the diff whole — split across two PRs, your measurement is wrong.

> **If your repo is one of the six reviewed by two groups**, work independently.
> Do not look at the other group's fork or PR until both are submitted. Those six
> repos are the only place we can tell "hard codebase" apart from "unlucky group",
> and comparing notes destroys that.

---

#### **Part C – Using an LLM**

**You may use ChatGPT, Claude, Copilot, or anything else.** There is no penalty.

**You must submit the artifacts.** Add a file `LLM-LOG.md` to your PR containing:

**1. Your setup**, at the top:

| | |
|---|---|
| Model(s) | e.g. GPT-5.1, Claude Opus 4.5, Gemini 3 Pro |
| Tool / harness | web chat, Copilot, Claude Code, Cursor agent, CLI |
| IDE / editor | VS Code, Visual Studio, CLion, Vim |
| Did you paste this assignment document into it? | y/n — and which parts |

**2. The session:**

1. Every prompt you sent, numbered and in order.
2. The relevant part of each reply — the code it produced, not the whole essay.
3. Which attempt finally worked, and what was wrong with the earlier ones.
4. **The total number of prompts it took to get working code.**
5. **What code you gave it.** Whole files, selected functions, or the whole
   repository? Name the files. If you had to hunt for which files to supply, say
   how you found them.

Why each of these is asked for:

- **Setup.** A web chat you paste into and an agent that reads the repo itself are
  not the same instrument. Prompt counts are only comparable within a tool.
- **The assignment document.** If you pasted it, the assistant already knew the
  four requirements and was not working them out from the code. That changes what
  your prompt count means. Saying so costs nothing; hiding it makes your row
  useless.
- **Prompt count (4).** Fifteen attempts on your codebase and three on someone
  else's is evidence about the two codebases, not about the assistant.
- **What code you gave it (5).** An assistant handed one whole file has the entire
  program in front of it and never has to go looking. One working across nine
  files only sees what you thought to give it. We cannot interpret your results
  without knowing which you were in.

A PR with LLM-written code and no `LLM-LOG.md` scores zero for Parts C and D.
Submitting the log costs you nothing and is the honest, easy path.

---

#### **Part D – The measurement and the analysis**

With your PR branch checked out:

```bash
# lines and files
git diff --shortstat -w main...feat/multiplayer
git diff --stat      -w main...feat/multiplayer

# functions -- git prints the enclosing function after each @@ marker
git diff -U0 -w main...feat/multiplayer | grep '^@@'
```

(Use `master` if that is the repo's default branch.)

The `grep` gives you a starting list. **Check it by hand** — git guesses the
enclosing function from indentation and gets it wrong on tightly packed code, and
it says nothing at all for lines changed outside any function. Lines you changed
at global scope still count as reach: note them as `(global)`.

Put this table in the **PR description**:

| | |
|---|---|
| **Functions changed** (the headline number) | |
| **Names of those functions** | |
| Files changed | |
| Lines added / deleted | |
| % of codebase touched | lines changed ÷ total lines |
| Hours your group spent | |
| LLM model + tool | e.g. Claude Opus 4.5 via Cursor; `none` |
| LLM prompts to working code | |
| What code you gave the LLM | whole file / selected functions / whole repo / none |
| Pasted this assignment doc into the LLM? | y/n |

**The number that counts is functions changed, not lines.** If `render()` assumes
there is one snake, then `render()` has to change — whether you edited it by hand
or an assistant rewrote it. The *reach* of the change is a property of the code.
The *volume* is mostly a property of whoever typed it, and an assistant that
rewrites a whole function to alter two lines will inflate your line count without
telling us anything. Report the lines, but do not read anything into them.

"Functions changed" means every function your diff touches, **including ones with
nothing to do with snakes** — rendering, scoring, input, file I/O. List them by
name. Those are the interesting ones.

Then answer, in **no more than 400 words**:

1. Which functions did you have to change that are **not about snakes at all**?
   Why did a second snake reach them?
2. Was there a **single place** where "there is exactly one snake" was written
   down — or was that assumption spread across the code?
3. In Lecture 3 your group predicted which functions would change. **Compare your
   prediction to what actually happened.** What did you miss, and why was it not
   visible from reading?
4. What **one design change** to this codebase would have made this a
   twenty-minute job?

Question 3 is worth the most, and here is why.

Your Lecture 3 prediction was made by hand, from reading, with no assistant and no
compiler — before anyone had touched this code. That makes it the **control** for
this whole exercise. Whatever you use to write the change today, that prediction
is a clean record of what the design told a human reader on sight.

So Q3 is not a formality. Reading the code and changing the code are different
instruments, and where they disagree is the finding. A design that hid three
functions from you until you tried to compile is a design that will hide them
from the next person too.

---

#### **Part E – Submission**

In the shared Google Sheet, find your group's row (`A01`–`A42`) and fill in all
four columns:

| Column | What to put |
|---|---|
| **PR Link** | the URL of your open pull request |
| **Could you build it? (y/n)** | honest answer; `n` costs no marks |
| **LLM prompts to working code** | a number, or `0` if you used none |
| **LLM context** | whole file / selected functions / whole repo / none |

The PR must be **open**, not merged, by **Wednesday 12:00 PM**. The sheet is what
gets marked — a PR that exists but is not linked there is not submitted.

> **If someone opens a PR against your repository:** review it, and leave at least
> two comments naming a file and a line. **Do not merge it. Do not close it.** The
> open PR is the artefact being assessed.
>
> Confirm in your review whether their second snake actually works.

---

#### **If it will not build**

Build and run it before you claim it works. If it genuinely will not compile —
missing file, broken code as submitted — say so in the PR description and in the
sheet, make the change by reasoning from the code, and ask the owning group to
verify it in their review. That costs no marks.

Do **not** port the game to another platform, swap out its input library, or fix
unrelated build errors. Those are far bigger changes than the one being measured,
and they destroy your diff.

---

#### **If you do not finish**

**Say so, and you lose no marks.** Push what you have, open the PR anyway, and in
Part D describe exactly where you got stuck and what was in the way.

Some of these codebases will make this a twenty-minute change. Others will resist
it all afternoon. That difference is the entire point of the exercise. It is a
property of a design decision someone made last year — not a measure of you today.

An honest *"we could not finish because the snake's position is held in four
global arrays read by eleven different functions, and we could not find them all"*
is a **better** answer than a rushed implementation with no analysis.

---

#### **Deliverables**

1. An **open Pull Request** to the reviewed repository, from your fork.
2. `LLM-LOG.md` in the PR, if you used an assistant.
3. The **measurement table** (Part D) in the PR description.
4. The **analysis** (Part D) in the PR description.
5. All **four sheet columns** filled in your group's row.
6. **Two review comments** on the PR opened against your own repository.

---

#### **Evaluation Criteria**

| Criteria                                                      | Marks  |
| ------------------------------------------------------------- | ------ |
| PR opened correctly; scope respected; diff free of noise      | 5      |
| Measurement table complete and accurate                        | 5      |
| Analysis Q1–Q2 — which functions were reached, and why        | 5      |
| Analysis Q3 — prediction vs reality                            | 10     |
| Analysis Q4 — the one design change that would have helped    | 5      |
| LLM log complete, if an assistant was used                     | 5      |
| Review comments on the PR against your own repo(ping instructor if your repo not in the sheet)                | 5      |
| **Total**                                                      | **40** |

Marks are for the **measurement and the analysis**. A working second snake with
no analysis scores below an unfinished attempt that explains itself well.
