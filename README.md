# IT643 — Software Design and Testing

> **Instructor:** Ankush Chander &nbsp;|&nbsp; **Institute:** Dhirubhai Ambani University

[![MkDocs Site](https://img.shields.io/badge/MkDocs-Live-blue)](https://it643.ankushchander.com)
[![License](https://img.shields.io/badge/License-CC%20BY--NC%204.0-green)](LICENSE)

---

## Framing Question

If an AI can write, debug, and refactor code on demand — **what is left for a software engineer to do?**

This course argues that the scarce skill is shifting from *producing code* to **choosing what to build and judging whether the machine got it right.**

---

## What You Will Learn

### Module 1: Requirements Discovery
- [0: Software Engineering in post LLM era](docs/lectures/Lecture0.md)
- [1: Requirement Engineering](docs/lectures/Lecture1.md)
- [2: Elicitation & Stakeholder Interactions](docs/lectures/Lecture2.md)
 ### Module 2: Taming Complexity
- [3: Deep modules vs Shallow modules](docs/lectures/Lecture3.md) 
- [3_b: Code review - exercise ](docs/lectures/Lecture3-demo.md)
- [4: Refactoring](docs/lectures/Lecture4.md)
- [5-6: Smells in the data](docs/lectures/Lecture5_6.md)

---

## Labs

| Lab | What you do | Marks | Due date |
|---|---|---|---|
| [1: Make it multiplayer](docs/labs/assignment-1.md) | Add a second player to another group's snake game, then measure what the change cost | 40 | Wednesday, 12:00 PM |

---

## How This Site Works

- **Lecture pages** — Full Markdown notes with interactive questions (`??? hint` blocks to click open).
- **Labs** — Assignment briefs with deliverables and marking criteria.
- **Slides** — Clickable presentation decks for select lectures (powered by reveal.js / mkdocs-slides).
- **Lecture Index** — Complete topic map at [lectures/README.md](docs/lectures/README.md).

## Prerequisites

- Familiarity with at least one programming language
- Basic understanding of how software systems work
- No prior formal software engineering training required

---

## Local Development

```bash
# Setup virtual environment (if not already done)
python -m venv .venv
source .venv/bin/activate  # On Windows: .venv\Scripts\activate

# Install dependencies
pip install -r requirements.txt

# Serve the site locally
mkdocs serve
```

The site will be available at `http://127.0.0.1:8000`. Changes to Markdown files are hot-reloaded automatically.

---
