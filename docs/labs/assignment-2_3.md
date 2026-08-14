#### **Problem Statement**

AI does the task it"s given but that doesnt gaurantee the design health of the codebase. That"s something you have to take into account.


This lab is about leveraging LLMs to optimise the design health of your codebase.

This week you put that second objective into the tool permanently, and then measure whether
it made any difference. You will work on **the same repository you modified in Lab 1**.

**Deadline: Monday(24/Aug/2026), 11:59 PM.**

---

#### **Part A – Install the skills**

A **skill** is a folder containing a `SKILL.md`: a name, a description telling the agent when
to use it, and a body of instructions. That is the entire mechanism. It is not a plugin, it
does not run code, and there is nothing to compile. It is a file the agent reads.

Both skills are pinned in the course repository so that all 42 groups run the same
instrument.:

| | |
|---|---|
| Install notes, per tool | [`skills/README.md`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/skills/README.md) |
| The glossary skill | [`skills/ubiquitous-language/`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/tree/main/skills/ubiquitous-language) |
| The code-smell pack | [`skills/refactoring/`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/tree/main/skills/refactoring) |
| Report template | [`lab2_3/REPORT-template.md`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/lab2_3/REPORT-template.md) |
| Submission checker | [`lab2_3/check-lab2_3.sh`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/lab2_3/check-lab2_3.sh) |

```bash
git clone https://github.com/Ankush-Chander/IT643-software-design-and-testing it643-content
cat it643-content/skills/README.md    # install paths per tool, and the gotcha below
```

While you have it, copy the two submission files into your fork — you will need them from
Part B onward:

```bash
mkdir -p lab2_3/audits
cp it643-content/lab2_3/REPORT-template.md lab2_3/REPORT.md
cp it643-content/lab2_3/check-lab2_3.sh    lab2_3/
```

If you would rather not clone, pull those two straight down instead:

```bash
mkdir -p lab2_3/audits
curl -L -o lab2_3/REPORT.md \
  https://raw.githubusercontent.com/Ankush-Chander/IT643-software-design-and-testing/main/lab2_3/REPORT-template.md
curl -L -o lab2_3/check-lab2_3.sh \
  https://raw.githubusercontent.com/Ankush-Chander/IT643-software-design-and-testing/main/lab2_3/check-lab2_3.sh
```

Install `ubiquitous-language` and the `refactoring/` pack. For Claude Code that is a copy:

```bash
cp -r it643-content/skills/ubiquitous-language ~/.claude/skills/
cp -r it643-content/skills/refactoring/*       ~/.claude/skills/
```

For any other tool, check its documentation for where skills live and copy them there.

**If your tool has no skill mechanism at all** — a web chat, or an IDE assistant that does
not support them — paste the contents of `SKILL.md` as your first message in the session.
A skill is a file. You lose the `/name` shortcut and nothing else. This costs you no marks;
say which route you took.

Note:
- It **never fires on its own.** It sets `disable-model-invocation: true`, so you have to
  ask for it by name. Most skills load automatically when their description matches what
   you are doing; this one is the exception. "I installed it and nothing happened" is the
   expected result of not knowing that.

Write your tool and your install route into **section 1** of `lab2_3/REPORT.md` now, while you
still remember which of the four things you tried was the one that worked. We take you at
your word on this — there is no screenshot to submit. If nothing would install and you used
the paste fallback, say that; it is a documented route and costs no marks.

---

#### **Part B – Name the things (`ubiquitous-language`)**

Get the source into the session, then invoke the skill. It writes `UBIQUITOUS_LANGUAGE.md`:
a term / definition / **aliases-to-avoid** table, plus a *flagged ambiguities* section naming
every place one word was used for two concepts, or two words for one.

Expect domain words — *Snake, Fruit, Board, Tick, Score, Collision*. The skill deliberately
skips class and function names, so `drawGame` and `gotoxy` will not appear. That is correct
behaviour, not a failure.

**Then edit the file.** This is the graded part. From the skill's own documentation:

> an unreviewed, agent-authored glossary is worse than none: it becomes confident-sounding
> lore that later sessions treat as truth.

Fix it: where it guessed wrong. Where it invented a term nobody uses, delete it.

**Deliverable:** save the edited file as `lab2_3/UBIQUITOUS_LANGUAGE.md` and write three or four
lines into **section 2** of `lab2_3/REPORT.md` saying what you changed and why. The file is the
artifact; section 2 is the graded part.

---

#### **Part C – Find the smells, before and after**

**Read [`review-accuracy-calibration`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/skills/refactoring/review-accuracy-calibration/SKILL.md) first.** It gives you a confidence model — C4 down to C1 — for deciding what is worth reporting. It also opens with the number that makes this lab worth your time: AI review tools cost developers *2–5 hours a week* in false positives, and about *25% of AI suggestions contain errors*. You are about to generate a pile of
suggestions. Some of them will be wrong.

Now run [`detect-code-smells`](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/skills/refactoring/detect-code-smells/SKILL.md)
at **two commits** and save the output:

| Commit | Report |
|---|---|
| `main` — the code as you received it | `lab2_3/audits/main.md` |
| your Lab-1 branch head | `lab2_3/audits/lab1-head.md` |

The filenames say which commit each report describes, so nobody reading them later has to
guess what "before" was before.

**House rules, because the skill will not impose them.** `detect-code-smells` is a catalog,
not a procedure: it hands the agent 23 smell names, their severities, and which refactoring
fixes each. It says nothing about how to search a repo or what a report should look like.
So insist, in your prompt:

- **Report only. Do not edit anything.**
- **Every smell cites `file:line`.** If it cannot point at a line, it is not a finding.
- **Assign each one a confidence level** from the calibration skill.

Then fill in the delta table in **section 3** of `lab2_3/REPORT.md` — introduced, left
untouched, removed. The third row will probably be zero. Leave it in. **The empty row is the
finding**, and it is worth more than the other two.

**Finally, reject at least three.** Go through what the agent reported and find three
candidates that are *not* real findings on this codebase — a C1 guess, a rule fired on code
that is fine, a "Large Class" on a class that genuinely needs those methods. One row each in
**section 4**: the smell, the `file:line`, and one sentence on why it does not hold.

This is not busywork and it is worth as much as the delta table. A group that accepts
everything the agent reported has learned to run a tool, and has reproduced — one level up —
exactly the failure this lab is about.

---

#### **Part D – Do it again, design first**

Two runs of the same feature. You have already done the first one.

**Run 1 — your Lab-1 branch.** IDE, no skills, no design step. Already measured. **Do not
re-run it.**

**Run 2 — skills loaded, design first.** Start a fresh branch from `main`:

```bash
git checkout main
git checkout -b lab2_3/design-first
```

Work in a **skill-capable CLI agent** (Claude Code, Codex CLI, Gemini CLI — whichever you can
install; the paste fallback from Part A is fine too). Make **four commits, in this order**:

| # | Do this | Commit |
|---|---|---|
| 1 | Let the agent read the source, then invoke `ubiquitous-language`; edit the result | `lab2_3/UBIQUITOUS_LANGUAGE.md` |
| 2 | *"Find the code smells in this repo. Report only — do not edit."* | `lab2_3/audits/main.md` |
| 3 | *"Refactor so that 'how many snakes there are' is written down in exactly one place. Do not add a second player yet. Do not change behaviour."* | the refactor, alone |
| 4 | *"Now add the second player."* — same four requirements as Lab 1, nothing more | the feature, alone |
| 5 | Fill in the report, run the checker | `lab2_3/REPORT.md`, `lab2_3/audits/lab1-head.md` |

Commits 1 and 2 are the Part B and Part C work — you are not redoing it, you are landing it
on this branch. Commit 5 is bookkeeping and can be as many commits as you like; **1 to 4 must
be four separate commits in that order.**

**Commit 3 and commit 4 must be separate commits.** That separation is the deliverable. In
Lab 1 the refactor and the feature — if any refactor happened at all — arrived as one
indivisible blob that a reviewer had to untangle. Here a reviewer can read the structural
change on its own, agree or disagree with it, and then read a feature diff that is only
about the feature.

Same scope discipline as Lab 1 in commit 4: no new menus, no colours, no tidying, no
reformatting.

Then run the checker from the root of your fork:

```bash
bash lab2_3/check-lab2_3.sh
```

It prints your commit table for **section 5** and the run-2 line counts for **section 6** —
measured the same way for all 42 groups, which is the only reason the class comparison will
mean anything. Paste both in, then fill in the run-1 column from your Lab-1 numbers.

For run 2, the feature rows are **commit 4 alone** — that is the row comparable to Lab 1.
Commit 3's size goes on its own line underneath.

> **On "functions reached".** We are reporting it because you measured it last time, but
> treat it carefully. A16 changed exactly one function in Lab 1 and said it best themselves:
> their repo is a single `main()`, *"so '1 function changed' is a flattering number rather
> than a good one."* A monolith cannot show a seam count because it has no seams. Smells
> introduced is the number that carries this lab.

**Do not push this to the Lab-1 pull request.** That PR is a measurement artifact and other
people are still being marked on it. Push `lab2_3/design-first` to **your own fork** and open
the pull request **against your fork's own `main`**. Link that PR in the sheet.

---

#### **Part E – The analysis**

Q1 and Q2 go in **section 7** of `lab2_3/REPORT.md`, Q3 and Q4 in **section 8**. All four
together, **no more than 500 words** — the checker counts them.

1. **Which smell did commit 3 actually fix?** Name it from your Part C report. State what was
   expensive before — *"adding a field meant editing four places"* — and what it costs now.
   Be concrete; this is the whole point of naming a smell rather than saying the code felt
   bad.

2. **Compare commit 4 to your Lab-1 diff.** Same feature, same codebase. What changed in the
   cost, and what did not? If it got *worse*, say so and explain what happened — that is a
   real result and it marks the same.

3. **Go back through your Lab-1 `LLM-LOG.md`. At any point, did the assistant suggest
   restructuring before adding the feature?** Quote it if it did. If it did not, say what you
   think would have had to be different in your prompt for it to have done so.

4. **How do you know commit 3 did not change behaviour?** Answer honestly. Most of you will
   find that you do not know — you played the game for a minute and it looked fine. Say that
   plainly if it is true, and describe what you would have needed in order to actually know.

Question 4 has no right answer this week, and it is not a trick. Sit with it. It is where
this course goes next.

---

#### **Part F – Submission**

**One pull request. Everything in it. Nothing anywhere else.**

> **This is different from Lab 1.** Last time the tables and the analysis went in the pull
> request description. This time they go in a **committed file**, `lab2_3/REPORT.md`. A commit
> is timestamped and a description is not; a file can be cloned, diffed and counted across
> the whole class and a description cannot. Put nothing in the PR description that is being
> marked.

Your branch adds exactly one directory to the repository:

```
lab2_3/
├── REPORT.md               ← the only thing a human reads. Eight sections, in order.
├── UBIQUITOUS_LANGUAGE.md  ← Part B, edited by you
├── audits/
│   ├── main.md             ← smells at main
│   └── lab1-head.md        ← smells after your Lab-1 PR
└── check-lab2_3.sh         ← the checker, copied from the course repo
```

The eight headings in `REPORT.md` **are** the marking scheme, in marking order. Do not rename
or reorder them. Everything you are graded on is in that file or in one of the three files it
points at.

**Before you open the PR, run the checker and get a clean exit:**

```bash
bash lab2_3/check-lab2_3.sh
```

It fails on a missing file, a missing or renamed heading, an unfilled `___`, fewer than three
rejected candidates, or fewer than four commits. Those are the things that would otherwise
cost you marks for reasons that have nothing to do with the work — so the check is yours to
run, not ours to chase. It also warns about smells reported without a `file:line` and an
over-length analysis; warnings do not block you.

Then open the PR **against your own fork's `main`** — not upstream, not the Lab-1 PR — and
fill in your row in the shared Google Sheet:

| Column | What to put |
|---|---|
| **PR link** | your `lab2_3/design-first` PR |
| **Tool** | Claude Code / Codex CLI / Gemini CLI / pasted `SKILL.md` / other |
| **Smells introduced: run 1** | a number |
| **Smells introduced: run 2** | a number |
| **Prompts: run 2** | a number |
| **Checker clean?** | yes / no |

Those five numbers are what the class distribution gets built from in the next lecture, the
same way Lab 1's numbers did. Nothing else belongs in the sheet — no prose, no tables.

The PR must be **open** by the deadline and **linked in the sheet**. As in Lab 1: a PR that
exists but is not in the sheet has not been submitted.

---

#### **If you do not finish**

**Say so, and you lose no marks.** Push what you have, open the PR anyway, and describe in
the last section of `lab2_3/REPORT.md` exactly where you got stuck. Put a dash in the table
cells you could not fill so the checker stops complaining about them. An honest *"the refactor in commit 3 broke the game and
we could not work out why, and here is what we tried"* is worth more than a polished run 2
with no analysis. That was true in Lab 1 and it produced the most useful submissions in the
class.

If a skill will not install on your machine, use the paste fallback and say so. That is a
documented route, not a failure.

---

#### **A note on the tool that does this for you**

The upstream pack these skills come from also ships a `refactor` orchestrator that runs the
whole pipeline by itself: detect, prioritise, split into parallel git worktrees, dispatch
subagents, merge whatever passes. It has a mode that skips every confirmation prompt.

It is not in the course copy, on purpose. It produces one undifferentiated result where this
assignment asks for a visible boundary between the refactor and the feature, and it makes
exactly the judgement calls — which smells are real, which one to fix — that you are being
marked on.

If you find it and use it anyway, **say so in section 1 of your report.** That is a finding, not
a violation, and we will read it with interest.

---

#### **Deliverables**

One PR from branch `lab2_3/design-first` against your own fork's `main`, containing:

1. `lab2_3/UBIQUITOUS_LANGUAGE.md` — edited by you.
2. `lab2_3/audits/main.md` and `lab2_3/audits/lab1-head.md`.
3. `lab2_3/REPORT.md` — all eight sections filled in.
4. Commits 1–4 in order, the refactor separate from the feature.

Plus your row in the sheet. That is the whole submission.

---

#### **Evaluation Criteria**

Each row is one section of `lab2_3/REPORT.md`, in order.

| § | Criteria | Marks |
|---|---|---|
| 1 | Skills installed; tool and install route stated | 3 |
| 2 | `UBIQUITOUS_LANGUAGE.md` committed and edited; what you changed and why | 4 |
| 3 | Both smell reports + delta table, every smell cited `file:line` | 6 |
| 4 | **Three candidates rejected, with a reason each** | 6 |
| 5 | Four commits, in order, with the refactor separate from the feature | 7 |
| 6 | Two-run measurement table, complete and honest | 4 |
| 7 | Analysis Q1–Q2 — which smell, what it cost, what changed | 5 |
| 8 | Analysis Q3–Q4 — what the assistant did not offer; how you know behaviour held | 5 |
| | **Total** | **40** |

Marks sit on the analysis and the rejections. A clean run 2 with no analysis scores below an
abandoned attempt that explains itself — same principle as Lab 1.
