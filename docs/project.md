# Project evaluation

**When:** immediately after the end-sem exam · lab viva, whole group present

**Submit:** tag your repo `project-final` before your slot.

---

## Ceiling

| Ceiling | You have | Evidence |
|---|---|---|
| **10** | deployed, hosted, used by people outside your team | live URL + evidence of outside use — analytics, feedback log, or user list |
| **9** | deployed MVP, key features working | live URL |
| **8** | anything else | — |

The ceiling is the maximum. Everything below decides your score within it.

---

## Four files, in your repo root

### 1. `README.md`

- live URL
- how to run it locally, in ≤ 5 commands

### 2. `UBIQUITOUS_LANGUAGE.md`

The words your product is about — *Order, Slot, Booking*, not *Manager, Handler, Util*.
Same format as Lab 2/3.

| Term | Definition | Aliases to avoid | Named in code at |
|---|---|---|---|

Then **flagged ambiguities**: every place one word means two things, or two words mean one.
Say which one wins.

### 3. `DESIGN.md`

Use the terms from `UBIQUITOUS_LANGUAGE.md`. A new word for an existing concept is a
finding against you.

**Five rules** your product enforces, in plain English.

**Three design decisions**, each with:

- the principle it applies — SOLID, a component principle, or a boundary
- `file:line` where it lives
- what it looked like before, and why you changed it

**One boundary** — where your core logic stops and the outside world begins, and which
dependency it protects you from.

**AI use**

- which tools you used, and for what
- **one AI suggestion you rejected**, and why

### 4. `TESTING.md`

| Rule | Test `file:line` | If untested — what stops you |
|---|---|---|

Then one line each:

- how many of your five rules are tested
- one test you would call weak, and why

A low count costs less than tests that assert nothing.

---

## Viva

Any member may be asked about any `file:line` in either document.

==Docs that don't match the code are graded as absent.==

Using AI costs nothing. Not being able to explain your own code does.

---

## Scoring within the ceiling

| | Weight |
|---|---|
| `UBIQUITOUS_LANGUAGE.md` + `DESIGN.md` — terms, decisions, citations, boundary | 35% |
| `TESTING.md` — rules tested, honesty about gaps | 35% |
| Viva — code matches docs, every member can explain | 30% |

---

## Checklist

- [ ] `project-final` tag pushed
- [ ] live URL in `README.md`
- [ ] `UBIQUITOUS_LANGUAGE.md` — terms + flagged ambiguities
- [ ] `DESIGN.md` — 5 rules, 3 decisions, 1 boundary, AI use
- [ ] `TESTING.md` — rule table filled
- [ ] every `file:line` still points at the right line
