---
name: refactoring-decision-matrix
description: Use when you have identified a code smell and need to select the right refactoring technique, assess risk, and prioritize the work — maps all 23 code smells to specific fix paths, difficulty levels, and risk ratings
---

Prerequisite: Run `detect-code-smells` first to identify issues, then use this matrix to select the fix.

# Refactoring Decision Matrix

## How to Use

1. Identify the smell from `detect-code-smells`
2. Find the smell row in the matrix below
3. Select fix based on context; check difficulty and risk
4. Apply the "When NOT to Refactor" rules before starting

---

## Decision Matrix: All 23 Smells

### Bloaters (5 smells)

| Smell | Primary Fix | Primary Skill | Secondary Fix | Secondary Skill | Difficulty | Risk |
|-------|------------|---------------|---------------|-----------------|------------|------|
| Long Method | Extract Method | `refactor-composing-methods` | Replace Method with Method Object | `refactor-composing-methods` | Easy | Low |
| Large Class | Extract Class | `refactor-moving-features` | Extract Subclass | `refactor-generalization` | Medium | Medium |
| Primitive Obsession | Replace Data Value with Object | `refactor-organizing-data` | Replace Type Code with State/Strategy | `refactor-organizing-data` | Medium | Medium |
| Long Parameter List | Introduce Parameter Object | `refactor-simplifying-method-calls` | Preserve Whole Object | `refactor-simplifying-method-calls` | Easy | Low |
| Data Clumps | Extract Class | `refactor-organizing-data` | Introduce Parameter Object | `refactor-simplifying-method-calls` | Easy | Low |

### Object-Orientation Abusers (4 smells)

| Smell | Primary Fix | Primary Skill | Secondary Fix | Secondary Skill | Difficulty | Risk |
|-------|------------|---------------|---------------|-----------------|------------|------|
| Switch Statements | Replace Conditional with Polymorphism | `refactor-simplifying-conditionals` | Replace Type Code with State/Strategy | `refactor-organizing-data` | Medium | Medium |
| Temporary Field | Extract Class (with Null Object) | `refactor-organizing-data` | Introduce Null Object | `refactor-organizing-data` | Medium | Medium |
| Refused Bequest | Replace Inheritance with Delegation | `refactor-generalization` | Extract Superclass (restructure hierarchy) | `refactor-generalization` | Hard | High |
| Alternative Classes with Different Interfaces | Rename Method + Extract Superclass | `refactor-generalization` | Move Method to align interfaces | `refactor-moving-features` | Medium | Medium |

### Change Preventers (3 smells)

| Smell | Primary Fix | Primary Skill | Secondary Fix | Secondary Skill | Difficulty | Risk |
|-------|------------|---------------|---------------|-----------------|------------|------|
| Divergent Change | Extract Class | `refactor-moving-features` | Move Method to separate concern clusters | `refactor-moving-features` | Medium | Medium |
| Shotgun Surgery | Move Method + Move Field (centralize) | `refactor-moving-features` | Inline Class to consolidate | `refactor-moving-features` | Hard | High |
| Parallel Inheritance Hierarchies | Move Method + Move Field to collapse one hierarchy | `refactor-generalization` | Replace Inheritance with Delegation | `refactor-generalization` | Hard | High |

### Dispensables (6 smells)

| Smell | Primary Fix | Primary Skill | Secondary Fix | Secondary Skill | Difficulty | Risk |
|-------|------------|---------------|---------------|-----------------|------------|------|
| Excessive Comments | Extract Method (comment becomes method name) | `refactor-composing-methods` | Rename Method/Variable | `refactor-composing-methods` | Easy | Low |
| Duplicate Code | Extract Method + Pull Up Method | `refactor-composing-methods` | Form Template Method | `refactor-generalization` | Medium | Low |
| Lazy Class | Inline Class | `refactor-moving-features` | Collapse Hierarchy | `refactor-generalization` | Easy | Low |
| Data Class | Move Method (add behavior to data class) | `refactor-organizing-data` | Encapsulate Field | `refactor-organizing-data` | Medium | Low |
| Dead Code | Delete it (verify with tooling) | `refactor-composing-methods` | Remove Parameter (if dead param) | `refactor-simplifying-method-calls` | Easy | Low |
| Speculative Generality | Collapse Hierarchy + Inline Class | `refactor-generalization` | Remove Parameter | `refactor-simplifying-method-calls` | Easy | Low |

### Couplers (5 smells)

| Smell | Primary Fix | Primary Skill | Secondary Fix | Secondary Skill | Difficulty | Risk |
|-------|------------|---------------|---------------|-----------------|------------|------|
| Feature Envy | Move Method (to the class it envies) | `refactor-moving-features` | Extract Method then Move Method | `refactor-moving-features` | Easy | Low |
| Inappropriate Intimacy | Move Method + Move Field | `refactor-moving-features` | Extract Class to encapsulate shared data | `refactor-moving-features` | Medium | High |
| Message Chains | Hide Delegate | `refactor-moving-features` | Extract Method + Move Method | `refactor-moving-features` | Easy | Low |
| Middle Man | Remove Middle Man | `refactor-moving-features` | Inline Method | `refactor-composing-methods` | Easy | Low |
| Incomplete Library Class | Introduce Local Extension (wrapper/subclass) | `refactor-moving-features` | Introduce Foreign Method (single utility) | `refactor-moving-features` | Medium | Low |

---

## Risk and Difficulty Levels

| Risk | Meaning | Precondition |
|------|---------|--------------|
| **Low** | Local change; one class or method affected | Basic test coverage |
| **Medium** | Multiple callers or inheritance involved | Integration tests on changed path |
| **High** | Public API changes, hierarchy restructuring, cross-module moves | Full test coverage + migration plan |

| Difficulty | Meaning |
|-----------|---------|
| **Easy** | Mechanical; IDE-automatable; < 30 min |
| **Medium** | Design judgment needed; multiple files; 30 min – 2 hours |
| **Hard** | Architectural reasoning; may need staged rollout; 2+ hours |

---

## When NOT to Refactor

Stop and reassess when:

1. **Code is about to be deleted or replaced.** No lasting benefit.
2. **No tests exist and adding them first is not feasible.** Refactor without a safety net converts smell into bug.
3. **Ship deadline is within 48 hours.** Log as tech debt instead.
4. **The "improvement" adds more complexity than it removes.** Measure before acting.
5. **Refactoring changes a public API without a migration plan.** Prepare deprecation path first.
6. **You are mid-refactoring.** Finish one, commit, then start the next.

---

## Prioritization Framework

### CRITICAL — Fix Now (before next commit)

Smells that hide bugs or block safe implementation:
- Shotgun Surgery blocking the feature you are adding
- Inappropriate Intimacy preventing test isolation
- Dead Code masking unreachable error branches
- Any HIGH-risk smell on today's change path

### HIGH — Fix During the Feature (same PR)

Smells slowing current work:
- Long Method in code you must modify
- Duplicate Code you are about to copy a third time
- Switch Statements you must extend
- Feature Envy in a method you are changing

### MEDIUM — Schedule Dedicated Time

Not on the immediate path but slowing the team:
- Large Class not involved in current feature
- Primitive Obsession for domain concepts in 3+ places
- Divergent Change in frequently-modified modules
- Alternative Classes causing code review confusion

### LOW — Fix Opportunistically

Cost comprehension but do not block work:
- Excessive Comments on stable code
- Lazy Class or Middle Man with no active development
- Speculative Generality in rarely-touched code
- Dead Code in well-tested modules

---

## Smell Frequency Heuristics

- **1 place**: fix opportunistically
- **3-5 places**: schedule dedicated time
- **6+ places**: treat as CRITICAL or HIGH regardless of category

Duplicate Code and Switch Statements are particularly dangerous at scale.

---

## Compound Smells

Certain smells co-occur. Detect one, search for its companions before refactoring.

| Primary Smell | Likely Companions | Root Cause |
|--------------|-------------------|------------|
| Large Class | Divergent Change, Duplicate Code | SRP violated |
| Long Method | Excessive Comments, Duplicate Code, Long Parameter List | Growth without decomposition |
| Primitive Obsession | Data Clumps, Long Parameter List | Missing value object |
| Switch Statements | Duplicate Code, Parallel Inheritance Hierarchies | Type code instead of polymorphism |
| Feature Envy | Message Chains, Inappropriate Intimacy | Behavior in wrong class |
| Shotgun Surgery | Duplicate Code, Data Clumps | Scattered responsibility |
| Refused Bequest | Temporary Field, Alternative Classes | Inheritance for reuse, not type relationships |

**Rule:** Fix the root cause smell first. Fixing companions alone leaves the structural problem intact.

---

## Refactoring Sequencing Rules

When multiple smells coexist, fix in this order:

1. **Dead Code** — remove noise before analyzing structure
2. **Duplicate Code** — consolidate before extracting
3. **Long Method / Large Class** — decompose once signal is clear
4. **Coupling smells** (Feature Envy, Inappropriate Intimacy, Message Chains) — move behavior after class boundaries stabilize
5. **Generalization smells** (Refused Bequest, Parallel Inheritance) — restructure hierarchies after classes are clean

Commit after each fix for reviewable history and surgical rollback.

---

## Cross-References

| Topic | Skill |
|-------|-------|
| Detecting all 23 smells with symptoms and triggers | `detect-code-smells` |
| Extract Method, Replace Temp with Query, Method Object | `refactor-composing-methods` |
| Move Method, Extract Class, Hide Delegate, Remove Middle Man | `refactor-moving-features` |
| Replace Conditional with Polymorphism, Decompose Conditional | `refactor-simplifying-conditionals` |
| Introduce Parameter Object, Replace Parameter with Method Call | `refactor-simplifying-method-calls` |
| Replace Data Value with Object, Encapsulate Field, Null Object | `refactor-organizing-data` |
| Extract Superclass, Replace Inheritance with Delegation | `refactor-generalization` |
| Pipeline composition, functional patterns | `refactor-functional-patterns` |
| End-to-end smell → anti-pattern → refactor → pattern examples | `pattern-detection-walkthroughs` |
