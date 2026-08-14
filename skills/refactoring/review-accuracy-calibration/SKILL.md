---
name: review-accuracy-calibration
description: Use when you want to improve the signal-to-noise ratio of any code review. Teaches confidence scoring, false positive reduction, severity calibration, and when to escalate vs. flag. Load this skill alongside review-cheat-sheet to ensure your findings are well-calibrated before posting comments.
---

# Review Accuracy and Calibration

## Overview

The accuracy problem in code review has two faces: over-flagging (false positives that waste reviewer and author time) and under-flagging (missing real defects). AI-assisted review tools generate false positives that waste 2-5 hours per developer per week, and 25% of AI suggestions contain errors. The fix is not reviewing less — it is calibrating more precisely.

This skill provides the meta-layer that makes every other review skill more effective: a confidence model, heuristics to suppress false positives, a severity calibration table, and an escalation decision guide. Load when filtering comments, assigning severity, or deciding whether to block a PR.

---

## Quick Reference — Confidence Levels

| Level | Label | Post? | Severity floor |
|-------|-------|-------|----------------|
| C4 — Certain | You have evidence: test failure, spec violation, data loss | Yes | HIGH or CRITICAL |
| C3 — High | Strong reasoning: well-known anti-pattern, measurable impact | Yes | MEDIUM or higher |
| C2 — Medium | Plausible concern but depends on context you lack | Conditional | LOW or NIT |
| C1 — Low | Speculative; could be intentional or context-dependent | No (investigate first) | — |

---

## Confidence Scoring Model

Assign a confidence level to every finding before posting it.

### C4 — Certain

You have direct evidence the code is wrong:
- A test fails or would fail if run
- The code violates a documented spec, contract, or schema
- Data loss or security exposure is provable (e.g., missing WHERE clause on DELETE, secret in source)
- The behavior contradicts the PR description

Action: Always post. Set severity to HIGH or CRITICAL. No hedge language needed.

```
// C4 example — provably wrong
DELETE FROM users   -- Missing WHERE clause: deletes ALL rows
// Post as CRITICAL. No ambiguity.
```

### C3 — High

Strong reasoning based on established patterns:
- A well-documented anti-pattern (N+1 query, mutable default argument, race condition on shared state)
- Clear performance or reliability impact measurable from the code
- Inconsistency with the existing codebase pattern (all other handlers do X; this one does Y)

Action: Post. Set severity to MEDIUM or HIGH. State your reasoning concisely.

```
// C3 example — established anti-pattern
def process(items=[]):   # Python mutable default — shared across calls
// Post as HIGH. Cite the pattern.
```

### C2 — Medium

Plausible concern but you lack full context:
- Pattern looks wrong but could be intentional (e.g., retry logic that looks infinite but may have a circuit breaker elsewhere)
- Performance concern that depends on data volume you cannot see
- Style inconsistency that may follow a team convention you are not aware of

Action: Post as LOW or NIT with conditional phrasing: "If X is true, consider Y." Do not block the PR on C2 alone.

```
// C2 example — depends on call site
function parseConfig(raw: string) {
  return JSON.parse(raw)  // No try/catch — may be intentional if caller guarantees valid JSON
}
// Post as LOW: "If this can receive untrusted input, wrap in try/catch."
```

### C1 — Low

Speculative, style-only, or easily explained by context:
- You do not understand the domain and the code might be correct
- The "issue" is a personal preference not backed by a rule or measurable impact
- You would need to read 5 more files to know if it is actually a problem

Action: Do not post. Investigate first. If investigation raises it to C2+, then post. If still C1, drop it.

---

## False Positive Reduction

Apply these heuristics before posting any comment.

### Heuristic 1: Check Framework Conventions First

Before flagging, verify it is not idiomatic for the framework. Django signals, React's empty-dep `useEffect`, Go's `if err != nil` chains, and Spring `@Autowired` fields are all intentional patterns.

Test: Search the codebase for the same pattern. If it appears 10+ times untouched, it is likely intentional.

### Heuristic 2: Read the Surrounding Code Before Flagging

A finding that looks wrong in isolation is often handled one function above or below (null check at entry point, global error middleware, retry wrapper at call site).

Test: Expand context by 20 lines in each direction before posting.

### Heuristic 3: Distinguish Language Idiom from Bug

| Language | Looks wrong | Is actually |
|----------|-------------|-------------|
| Go | `err` returned without wrapping | Idiomatic if no context to add |
| Python | No explicit type check on input | Duck typing; often correct |
| JavaScript | `==` instead of `===` | Bug (almost always); flag it |
| Rust | `.unwrap()` in tests | Acceptable; flag in production code only |
| Java | Checked exception not caught | Compile error; CI catches it, not you |

Test: Ask "would a senior engineer in this language agree this is wrong?"

### Heuristic 4: Require Measurable Impact for Performance Findings

Post if: You can estimate a query count, latency impact, or known O(n²) pattern on unbounded input.

Do not post if: You think it "might be slow" without a concrete model of why. Mark as C1 and drop.

### Heuristic 5: Separate Style from Correctness

Style findings must never be posted at MEDIUM or higher severity.

- CRITICAL/HIGH: correctness only — data loss, security, logic errors
- MEDIUM: design or maintainability (not blocking, but has a measurable cost)
- LOW/NIT: style, preference, minor improvements

Test: Would this finding cause a bug or incident if left unfixed? If no, it is LOW or NIT at most.

### Heuristic 6: Verify the Finding Against Test Coverage

If the code path is covered by a passing test, lower your confidence by one level.

- Code path has a test → start at C3 max
- Code path has no test → C4 is still possible

### Heuristic 7: Time-Box Investigation Before Posting

If you cannot confirm a C2 finding within 3 minutes, either post a question or drop it.

Post a question: "Is there a reason X does not handle Y? Wondering if this is intentional."

---

## Severity Calibration Matrix

| Finding Type | C4 — Certain | C3 — High | C2 — Medium | C1 — Low |
|---|---|---|---|---|
| Security (injection, auth bypass, secret exposure) | CRITICAL | HIGH | LOW (ask) | Drop |
| Data loss (destructive write, missing rollback) | CRITICAL | HIGH | LOW (ask) | Drop |
| Logic error (incorrect behavior, wrong output) | HIGH | HIGH | LOW | Drop |
| Error handling (unhandled exception, swallowed error) | HIGH | MEDIUM | NIT | Drop |
| Performance (N+1, unbounded query, blocking hot path) | HIGH | MEDIUM | NIT | Drop |
| API contract (breaking change, missing versioning) | HIGH | MEDIUM | LOW | Drop |
| Concurrency (race condition, deadlock risk) | HIGH | MEDIUM | LOW | Drop |
| Testing (missing test, untested edge case) | MEDIUM | MEDIUM | NIT | Drop |
| Design / architecture (coupling, SRP violation) | MEDIUM | LOW | NIT | Drop |
| Code smell (long method, duplication) | LOW | LOW | NIT | Drop |
| Style / naming / formatting | NIT | NIT | NIT | Drop |

**Key rule:** Never post a C1 finding as a change request. Never post a style finding above NIT. Never post a security finding below HIGH if it is C3+.

---

## When to Escalate vs. Flag

```
Is the finding C4 (Certain) or C3 (High)?
  NO  → Post as LOW/NIT or drop. Do not block the PR.
  YES → Continue.

Is the category Security, Data Loss, or Breaking API Contract?
  YES → CRITICAL or HIGH. Block the PR. Do not approve until resolved.
  NO  → Continue.

Would leaving this unfixed cause a production incident within 30 days?
  YES → HIGH. Request changes. Do not approve.
  NO  → Continue.

Would leaving this unfixed increase maintenance cost or defect risk?
  YES → MEDIUM. Request changes or leave a note. Approve is possible.
  NO  → LOW or NIT. Suggest. Do not block.
```

### Escalation examples

**Escalate to CRITICAL** (block immediately):
- Hardcoded API key in source code (C4, security)
- `DELETE FROM orders` with no WHERE clause (C4, data loss)
- JWT signature verification removed (C4, auth bypass)

**Escalate to HIGH** (request changes, do not approve):
- N+1 query on an endpoint that serves 10K requests/hour (C3, performance, measurable)
- Missing error branch on a payment API call (C3, error handling, incident risk)
- Removed required field from a public API without deprecation (C3, contract break)

**Flag as MEDIUM** (suggest, can approve):
- Function is 80 lines with two responsibilities (C3, design, maintainability cost)
- Retry logic has no backoff (C2, performance, may not be a problem at current load)

**Flag as LOW/NIT** (suggest, do not block):
- Variable name is ambiguous but the code is correct (C3, style)
- Missing JSDoc on an internal utility (C2, documentation)

---

## Cross-Language Calibration

### TypeScript / JavaScript — FP risk: Medium (dynamic types)
- `any` type: flag at MEDIUM only if on a public API boundary; internal `any` is LOW
- `==` vs `===`: flag at HIGH — type coercion bugs are real
- `async/await` without try/catch: flag at MEDIUM if the function can throw
- Missing `.catch()` on a Promise: HIGH if the Promise rejects on network failure

### Python — FP risk: High (duck typing)
- Missing type hint: NIT only; not a correctness issue
- Mutable default argument (`def f(x=[])`): HIGH — always a bug
- Bare `except:`: MEDIUM — suppresses all exceptions including KeyboardInterrupt
- `is` vs `==` on strings: MEDIUM in production, LOW in tests

### Go — FP risk: Low (explicit error returns)
- Ignored error return (`_`): HIGH for operations that can fail (file I/O, DB); NIT for operations that cannot
- Goroutine without `WaitGroup` or `done` channel: HIGH if the goroutine outlives the function
- `context` not propagated: MEDIUM if the function does I/O; NIT otherwise
- `defer` in a loop: HIGH — deferred calls accumulate until function exit, not loop iteration

### Java — FP risk: Low (checked exceptions, static typing)
- Raw types (`List` vs `List<T>`): MEDIUM — loses type safety at runtime
- Catching `Exception` or `Throwable`: MEDIUM — too broad; cite the specific exception
- `==` on objects: HIGH — reference equality, not value equality
- Synchronized on `this`: LOW — usually a design smell but rarely a direct bug

---

## Anti-Patterns in Calibration

### Over-Flagging

**Style-as-bug**: Posting naming conventions or formatting as HIGH or CRITICAL. Trains authors to ignore all comments.

**Speculative performance**: Flagging code as "probably slow" without a model of why. Every loop is not an N+1.

**Framework ignorance**: Flagging idiomatic patterns without checking. Causes distrust of the reviewer.

**Confidence inflation**: Posting a C2 finding as C4. Destroys credibility when the author explains the context.

### Under-Flagging

**Security minimization**: Treating a potential injection or auth bypass as LOW because "it probably won't be exploited."

**Complexity tolerance**: Accepting 200-line functions because "it works." Design issues compound.

**Test blind spot**: Not checking whether flagged behavior is tested. Missing test coverage is itself a MEDIUM finding.

---

## Calibrated vs. Uncalibrated Review Comments

### Example 1: TypeScript — Error Handling

**Uncalibrated (C1 posted as HIGH):**
> HIGH: This async function should handle errors.

**Calibrated (C3, MEDIUM with context):**
> MEDIUM: `fetchUser` is called without `try/catch` and the caller does not handle rejection (line 47 passes the Promise directly to the template). If the API is unavailable, the unhandled rejection will crash the Node process. Wrap in try/catch or add `.catch()` at the call site.

### Example 2: Python — Performance

**Uncalibrated (C1 posted as MEDIUM):**
> MEDIUM: This loop might be slow.

**Calibrated (dropped — C1):**
Investigation shows the list has a max of 20 items (bounded by the API's page size). No measurable performance impact. Drop.

### Example 3: Go — Goroutine Leak

**Uncalibrated (C2 posted vaguely):**
> Consider adding a done channel here.

**Calibrated (C3, HIGH with evidence):**
> HIGH: The goroutine on line 83 has no termination signal. If `processQueue` returns early (e.g., context cancelled), the goroutine continues running and holds the database connection open. Add `ctx.Done()` case to the select, or pass the context to `processQueue`.

---

## Cross-References

- `review-cheat-sheet` — Master reference; use calibration from this skill when assigning severity in Phases 1-3
- `review-code-quality-process` — End-to-end review workflow; apply confidence scoring before each comment in the deep-dive phase
- `security-patterns-code-review` — Security findings always start at C3+ minimum; never downgrade below HIGH for confirmed security issues
- `performance-anti-patterns` — Cross-reference before posting performance findings to confirm the pattern is real, not speculative
- `language-specific-idioms` — Cross-reference before flagging language-specific patterns to avoid framework ignorance false positives
