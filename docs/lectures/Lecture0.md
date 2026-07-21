# Software Engineering in post LLM era

> Framing question for the course: if an AI can write, debug, and refactor code
> on demand, what is left for a software engineer to do — and how do we get good
> at *that*?

By early 2026, AI-assisted coding has moved from experiment to default across most
professional software teams. The three readings below argue, from different angles,
that the *craft* of programming is changing rather than disappearing — the scarce
skill is shifting from producing code to **choosing what to build and judging whether
the machine got it right**.

---

## How LLMs have changed the Software Engineering industry

### Small teams augmented by agents
- The job description is moving up the stack. As Boris Cherny (head of Claude Code)
  puts it in the NYT piece, *"a coder is now more like an architect than a
  construction worker"* — you shape how features fit together while agents lay the
  bricks.
- Most developers still call themselves developers, but now only rarely write code by
  hand. They spend their day describing intent in plain English and reviewing the
  agent's *plan* and output. Tasks that took days can collapse to minutes (one NYT
  example: new customer code shipped in about half an hour).
- The circle of people who can produce software is widening. The NYT observes that
  *"the realms of programmers and everyday people… are drifting closer together,"* and
  economist Erik Brynjolfsson notes: *"Maybe they don't label themselves as software
  engineers, but they're creating code."* Generating code is getting cheap; turning
  code into reliable systems is not.

### Greenfield vs brownfield companies
- **Greenfield** work — new codebases, prototypes, throwaway tools — is where agents
  shine: little existing context to load, fast iteration, cheap-to-check output. This
  is where "vibe-coding" makes shipping trivial.
- **Brownfield** work — large legacy systems with implicit constraints, scale, and
  failure modes — is where the value concentrates. Osmani's rule of thumb: an agent
  gets you ~70% of a feature quickly, and the last 30% (gnarly edge cases, the right
  architecture, the right taste) *is the whole game*. Deep domain and system context is
  exactly what a prompt cannot improvise.

### AI as another level of abstraction
- Programming has always advanced by adding abstraction layers — machine code →
  assembly → high-level languages → frameworks → **natural-language intent**. Natural
  language is now a (leaky) interface to the machine.
- This is the software-team echo of Sutton's **Bitter Lesson**: general methods that
  scale with compute reliably beat hand-crafted, knowledge-encoded ones. Encoding what
  we already know *feels* productive but builds a ceiling; letting the optimizer
  discover builds a ladder. Osmani draws the career corollary — *there's no point
  solving the easy version of a problem; the value is in the hard version.*
- The right place for human knowledge moves too: not into hand-written solution
  detail, but into **problem setup** — specifying intent, curating the data, and
  defining how correctness is verified.

### Skepticism about AI
- **Loss of craft.** An (anonymous) Apple engineer in the NYT: *"I believe that it can
  be fun and fulfilling and engaging, and having the computer do it for you strips you
  of that."* That the quote required anonymity hints at how many dissenting voices go
  unheard inside AI-embracing companies.
- **"Liberation" is contested.** The Hacker News reaction pushed back not on whether
  the tools work, but on the framing: writing prompts and reviewing plausible-looking
  output for subtle errors is *still labor* — it can feel like micromanaging a chatbot,
  just with less typing.
- **Infrastructure dependency.** Toolchains that once ran locally now route through a
  handful of VC-backed firms (Anthropic, OpenAI) on monthly subscriptions. Opting out
  carries a real capability penalty — a structural break from the open, local
  developer stack of the past decades.

---

## How LLMs have changed how online forums, open source collaboration works

### Steady decline of Stack Overflow
- The classic loop — hit an error, search, land on a Stack Overflow answer — is being
  replaced by asking a model in-editor. The answer is instant, contextual to *your*
  code, and never says "closed as duplicate."
- A consequence worth naming in class: fewer new public Q&A means the training and
  reference commons that made these tools good may stop being replenished at the old
  rate.

### Decline of online forums
- Q&A that used to happen in public (forums, mailing lists, chat) increasingly happens
  in private conversations with an assistant. Programmers have an advantage here that
  Simon Willison flags in the NYT: they can *tether the AI to reality* by demanding the
  code actually run — *"If you're a lawyer, you're screwed, right?"* — so the private
  loop can self-check in a way other professions can't.

### Disruption of open source collaboration
- Osmani's reminder that the durable payoff of open source was never the pay — it was
  **reputation and relationships that compound**. In a world where shipping is cheap,
  the scarce move is choosing something worth shipping and doing it *in public, near
  the hard problems.*
- At the same time, the substrate is shifting: the developer platform is increasingly
  a proprietary model API rather than a shared, open toolchain — raising real questions
  about who controls the terms of collaboration.

---

## What it takes to thrive in the post LLM era?

### Problem finding vs problem solving
- As agents absorb the solving, *selecting* the problem becomes the scarce skill.
  Osmani: the strong practitioners *"bring judgment and intuition to the work; the rest
  bring a prompt."*
- Own the outcome. *"The AI did it" is not a defense* — your name is on the change.
  Practice **specification and verification** as distinct skills: write precise specs
  (clear thinking made legible), and never let the agent grade its own homework.

### Developing a judgment and taste for code quality
- Taste is pattern-matching — but the pattern-matching *has to be earned by doing the
  work.* Agents automate the boilerplate-and-bug-fixing reps that used to build that
  intuition, so you must seek deliberate practice on purpose.
- The real risk isn't the agent writing bad code; *it's losing the ability to tell.*
  Judgment atrophies quietly because the output keeps looking like working code.
- Deliberate practice: pick a few problems that matter and do them the hard way, build
  deep mental models of at least one system end-to-end, and keep a private log of every
  time an agent suggests something that looks wrong.

### Mastering code review without having written much code
- New default posture: *read a thousand times more code than you write,* and *treat
  every diff from an agent like a human review you need to carefully justify.*
- The thriving engineers won't be the fastest at accepting suggestions — they'll be
  *the ones who know instantly when to say no.*
- "Sprint the last mile": first drafts are free, so **finish is the product** — the
  extra polish on the hard 30% is the value only you can add.

---

## Key takeaways
- Programming isn't ending; the center of gravity is moving from writing code to
  **framing problems, verifying output, and owning outcomes.**
- Anything gradeable by an answer key gets automated — your career is the *ungradeable*
  part: what's worth building, and whether it's actually good.
- Foundations matter *more*, not less: algorithms, systems, data, and architecture are
  what let you review and trust what the machine produces.

---

## References
- [Coding After Coders: The End of Computer Programming as We Know It](https://www.nytimes.com/2026/03/12/magazine/ai-coding-programming-jobs-claude-chatgpt.html?unlocked_article_code=1.SlA.DBan.wbQDi-hptjj6) — Clive Thompson, *NYT Magazine* (unlocked link)
- [The Agent-Era Career](https://addyosmani.com/blog/career-advice-age-of-agents) — Addy Osmani
- [The Bitter Lesson Keeps Winning](https://pytholic.github.io/posts/the-bitter-lesson/index.html) — Raja Haseeb
