# Vendored agent skills

Skills used by the labs, pinned here so that every group runs the same instrument.

A **skill** is a folder with a `SKILL.md` inside it: a name, a description telling the agent
when to use it, and a body of instructions. That is the whole mechanism. Nothing here is
executable, and nothing here needs installing in the usual sense — it is a file the agent
reads.

We keep our own copy rather than pointing at an upstream URL because skills are software and
they rot. `ubiquitous-language` is the demonstration: it was **deleted from its upstream
repository on 2026-08-05**, so the install command still published on mirror sites does not
work. Ours does.

---

## `ubiquitous-language`

Extracts a domain glossary from the current session **and from the repository** and writes
`UBIQUITOUS_LANGUAGE.md` — a term / definition / **aliases-to-avoid** table, plus a "flagged
ambiguities" section naming every place one word was used for two concepts, or two words for
one, plus a "code drift" section naming every place the code calls a concept something the
domain does not.

Our copy reads both sources by default. Narrow it with `/ubiquitous-language <path>`,
`--conversation` (session only, the upstream behaviour) or `--code` (repository only).

### The one thing that will otherwise waste your afternoon

-  **It never fires on its own.** The skill sets `disable-model-invocation: true`
   (`allow_implicit_invocation: false` for Codex). You have to ask for it by name. Most
   skills load automatically when their description matches what you are doing; this one is
   the exception, and "I installed it and nothing happened" is the expected result of not
   knowing that.

It also skips class and module names and generic programming terms by design. On a game
you should expect *Snake, Fruit, Board, Tick, Score, Collision* — not `drawGame` or `gotoxy`.

### Installing it

**Claude Code** — copy the folder to either location and restart the session:

```bash
# just you, every project
cp -r skills/ubiquitous-language ~/.claude/skills/

# or: this project only, checked in with the repo
mkdir -p .claude/skills && cp -r skills/ubiquitous-language .claude/skills/
```

Verify by typing `/ubiquitous-language` — it should appear in the command list.

**Any other tool.** Skill folders are not yet portable across harnesses, and the directory
differs per tool. Check your tool's documentation for where skills live, then copy the folder
there. `agents/openai.yaml` carries the equivalent metadata for Codex.

**Fallback, and it is a real one:** if your tool has no skill mechanism at all — a web chat,
or an IDE assistant that does not support them — paste the contents of `SKILL.md` as your
first message in the session. A skill is a file. You lose the `/name` shortcut, and — in a
tool with no filesystem access — the codebase half, which degrades to a `--conversation` run.

### Provenance

| | |
|---|---|
| Upstream | [`mattpocock/skills`](https://github.com/mattpocock/skills) |
| Path | `skills/deprecated/ubiquitous-language/` |
| Recovered from | commit `c66bdee` (parent) — the commit that removed it, 2026-08-05 |
| Last upstream change | `697d4ce`, 2026-07-13 |
| Files | `agents/openai.yaml` — byte-identical to upstream. `SKILL.md` — **modified locally**, see below |
| Licence | MIT, © 2026 Matt Pocock — see `LICENSE` |

**Local modification to `SKILL.md`.** Upstream reads the conversation only and never opens a
file. Ours adds the repository as a second source: a `Scope` section with the flags above, a
`Reading a codebase` section ordering where to look (domain prose → types and schemas → enums
and state machines → API surface → test names → comments → identifiers, skipping vendored and
generated code), an `In code` column carrying a `file:line` for each term, and a `Code drift`
table listing concepts whose code name is not the canonical one — including concepts nobody
has implemented at all. Upstream's structure, rules and example dialogues are otherwise
intact, and the divergence is marked in a comment at the foot of the file.

The governing rule there is *code is evidence, not vocabulary*: `OrderRepository` tells you
**Order** is a domain term; it does not make `OrderRepository` one. The skill reports drift,
it never renames.

Upstream removed it rather than deprecating it; its job moved into a larger `domain-modeling`
skill that also maintains architecture decision records. We use the smaller original because
the glossary is the part the lab needs.

**A warning from that same upstream documentation, worth taking seriously before you commit
the file it generates:**

> an unreviewed, agent-authored glossary is worse than none: it becomes confident-sounding
> lore that later sessions treat as truth.

Read what it writes. Correct it. The lab asks you to record what you changed.

---

## `refactoring/` — the code-smell pack

Ten skills, vendored as a set. `detect-code-smells` is the one you invoke; the rest are what
it points at once it has found something.

| Skill | What it is for |
|---|---|
| `detect-code-smells` | **Start here.** The catalog: 23 smells in 5 families, each with a severity and a pointer to the technique that fixes it |
| `refactoring-decision-matrix` | Choosing between techniques when more than one would work |
| `review-accuracy-calibration` | The C1–C4 confidence model. **Read this before you write a report** |
| `refactor-composing-methods` | Extract Method, Inline, Replace Temp with Query — for Long Method, Duplicate Code |
| `refactor-moving-features` | Move Method/Field, Extract Class — for Divergent Change, Shotgun Surgery, Feature Envy |
| `refactor-organizing-data` | Replace Data Value with Object, Encapsulate Field — for Primitive Obsession, Data Class |
| `refactor-simplifying-conditionals` | Replace Conditional with Polymorphism — for switch-on-type-code |
| `refactor-simplifying-method-calls` | Introduce Parameter Object, Preserve Whole Object |
| `refactor-generalization` | Extract/Collapse Hierarchy, Replace Inheritance with Delegation |
| `cpp-review-patterns` | RAII, const correctness, move semantics, UB traps — the C++ lens |

The catalog is Fowler's, by way of refactoring.guru. You already have the vocabulary: Long
Method, Duplicate Code, Divergent Change, Shotgun Surgery, Comments — these are the smells
from Lectures 4 and 5/6, with the same names.

### What the skill does not do

`detect-code-smells` is a **catalog, not a procedure**. It gives the agent the names, the
severities and the flowchart. It does not say how to search a repository, it does not require
a `file:line` citation, and it does not define a report format. The assignment does that —
and the difference between the two is worth noticing. A skill that names things well still
leaves the discipline to you.

So expect to have to insist: *report only, do not edit; cite `file:line` for every smell; if
you cannot point at a line, it is not a finding.*

### Why `refactor` is not here

The upstream pack ships a `refactor` orchestrator that runs the whole pipeline by itself —
detect, prioritise, split into parallel git worktrees, dispatch subagents, merge the branches
that pass. It has a mode that skips every confirmation prompt.

It is deliberately excluded. That button is the subject of this lab, not a tool for it: it
produces one undifferentiated result where the assignment asks for a visible boundary between
*the refactor* and *the feature*, and it makes the judgement calls that are the marked part
of your work. If you find it and install it anyway, say so in your submission — that is a
finding too, not a violation.

### Provenance

| | |
|---|---|
| Upstream | [`mickeyyaya/refactoring-skills`](https://github.com/mickeyyaya/refactoring-skills) |
| Commit | `cd0c227` (2026-03-17) |
| Vendored | 10 of 76 skills, file-identical to upstream; `refactor` excluded on purpose |
| Licence | MIT, © 2026 mickeyyaya — see `refactoring/LICENSE` |

A few "see also" lines in `cpp-review-patterns` and `refactoring-decision-matrix` point at
skills we did not vendor (`concurrency-patterns`, `error-handling-patterns`,
`rust-review-patterns`, `refactor-functional-patterns`, `pattern-detection-walkthroughs`).
Those references are prose, not dependencies — nothing breaks. Pull them from upstream if you
want them.

### Installing

Same as above — copy the folders you need:

```bash
cp -r skills/refactoring/* ~/.claude/skills/
```

These load automatically when their description matches what you are doing; unlike
`ubiquitous-language`, you do not have to invoke them by name.
