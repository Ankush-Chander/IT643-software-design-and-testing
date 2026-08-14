---
name: ubiquitous-language
description: Extract a DDD-style ubiquitous language glossary from the current conversation and/or the codebase, flagging ambiguities, proposing canonical terms, and reporting where the code's names have drifted from the domain's. Saves to UBIQUITOUS_LANGUAGE.md. Use when user wants to define domain terms, build a glossary, harden terminology, create a ubiquitous language, or mentions "domain model" or "DDD".
disable-model-invocation: true
---

# Ubiquitous Language

Extract and formalize domain terminology into a consistent glossary, saved to a local file.

Two sources feed the glossary:

- **The conversation** — what the humans in the session actually said.
- **The codebase** — what the source, schemas, tests and docs actually name things.

They disagree more often than anyone expects, and that disagreement is the most useful thing
this skill produces.

## Scope

Read **both sources by default**: the conversation so far, plus the repository rooted at the
working directory.

The invocation may narrow this:

| Invoked as                            | Read                                        |
| ------------------------------------- | ------------------------------------------- |
| `/ubiquitous-language`                | Conversation + repository at the working directory |
| `/ubiquitous-language <path>`         | Conversation + only that file or directory  |
| `/ubiquitous-language --conversation` | Conversation only — do not read files       |
| `/ubiquitous-language --code [<path>]`| Codebase only — ignore the conversation     |

If one source is empty, say so in the summary and proceed with the other. A fresh session
with no discussion is a normal `--code` run, not a failure. A session with no repository is a
normal `--conversation` run.

## Process

1. **Scan the conversation** for domain-relevant nouns, verbs, and concepts
2. **Scan the codebase** for the same, following "Reading a codebase" below
3. **Identify problems**:
   - Same word used for different concepts (ambiguity)
   - Different words used for the same concept (synonyms)
   - Vague or overloaded terms
   - A concept the humans call one thing and the code calls another (**drift**)
4. **Propose a canonical glossary** with opinionated term choices
5. **Write to `UBIQUITOUS_LANGUAGE.md`** in the working directory using the format below
6. **Output a summary** inline in the conversation

## Reading a codebase

**Code is evidence, not vocabulary.** You read identifiers to find out which concepts exist
and what they are called; you do not copy identifiers into the glossary. `OrderRepository` is
evidence that **Order** is a domain term. It is not itself a domain term.

Read in this order — the list is sorted by signal, and the first three are usually enough:

1. **Domain prose** — `README`, `docs/`, ADRs, specs, issue and PR templates. Humans writing
   for humans, so the vocabulary is already the domain's.
2. **Types, models, entities, schemas** — type definitions, ORM models, database migrations,
   protobuf/OpenAPI/JSON schema. The nouns of the system.
3. **Enums and state machines** — the highest-signal source of *lifecycle* vocabulary.
   `status: draft | confirmed | fulfilled | cancelled` is a domain lifecycle stated outright.
4. **The API surface** — route paths, RPC method names, CLI subcommands, event and message
   names, queue topics. The verbs of the system.
5. **Test names** — `it("refuses to invoice an unfulfilled order")` often states a domain
   rule in a full English sentence. Mine these for relationships and constraints.
6. **Comments and docstrings** on core modules.
7. **Identifier names** in the core modules only, once the above is exhausted.

Skip, always: vendored dependencies, `node_modules`, build and `dist` output, generated code,
lockfiles, fixtures, and minified assets. Skip framework and architecture vocabulary —
`Controller`, `Repository`, `Service`, `Manager`, `DTO`, `Factory`, `Handler`, `Util` — unless
the word genuinely carries domain meaning in this domain. (In a warehouse system, **Handler**
might. In most systems it does not.)

Cite what you find as `file:line`. A term you cannot cite and that nobody said out loud is a
term you invented — drop it.

## Output Format

Write a `UBIQUITOUS_LANGUAGE.md` file with this structure. Omit the **In code** column and the
**Code drift** section when the run was conversation-only.

````md
# Ubiquitous Language

## Order lifecycle

| Term        | Definition                                              | Aliases to avoid      | In code                    |
| ----------- | ------------------------------------------------------- | --------------------- | -------------------------- |
| **Order**   | A customer's request to purchase one or more items      | Purchase, transaction | `Order` — `models/order.py:14` |
| **Invoice** | A request for payment sent to a customer after delivery | Bill, payment request | `Bill` — `billing/bill.py:8` ⚠ |

## People

| Term         | Definition                                  | Aliases to avoid       | In code                        |
| ------------ | ------------------------------------------- | ---------------------- | ------------------------------ |
| **Customer** | A person or organization that places orders | Client, buyer, account | `Client` — `models/client.py:6` ⚠ |
| **User**     | An authentication identity in the system    | Login, account         | `User` — `auth/user.py:11`     |

## Relationships

- An **Invoice** belongs to exactly one **Customer**
- An **Order** produces one or more **Invoices**

## Example dialogue

> **Dev:** "When a **Customer** places an **Order**, do we create the **Invoice** immediately?"
> **Domain expert:** "No — an **Invoice** is only generated once a **Fulfillment** is confirmed. A single **Order** can produce multiple **Invoices** if items ship in separate **Shipments**."
> **Dev:** "So if a **Shipment** is cancelled before dispatch, no **Invoice** exists for it?"
> **Domain expert:** "Exactly. The **Invoice** lifecycle is tied to the **Fulfillment**, not the **Order**."

## Flagged ambiguities

- "account" was used to mean both **Customer** and **User** — these are distinct concepts: a **Customer** places orders, while a **User** is an authentication identity that may or may not represent a **Customer**.

## Code drift

Places where the code's name for a concept is not the canonical term. Reported, not renamed.

| Canonical term | Called in code | Location              | Note                                                          |
| -------------- | -------------- | --------------------- | ------------------------------------------------------------- |
| **Invoice**    | `Bill`         | `billing/bill.py:8`   | Whole `billing/` module uses "bill"; the domain says invoice   |
| **Customer**   | `Client`       | `models/client.py:6`  | "Client" also means HTTP client in `net/` — two concepts, one word |
| **Fulfillment**| *(absent)*     | —                     | Discussed as a lifecycle stage; no code represents it          |
````

## Rules

- **Be opinionated.** When multiple words exist for the same concept, pick the best one and list the others as aliases to avoid.
- **The humans win.** When the conversation and the code name the same concept differently, the term the domain expert used is canonical and the code is what has drifted. Record it under "Code drift".
- **Report, never rename.** This skill writes one Markdown file. It does not edit source, and it does not propose a patch. Renaming is a separate, reviewed decision.
- **Cite your evidence.** Every term traces to something someone said or to a `file:line`. If you can point at neither, it is not a term — it is an invention, and an invented glossary is worse than no glossary.
- **Flag conflicts explicitly.** If a term is used ambiguously in the conversation or in the code, call it out in the "Flagged ambiguities" section with a clear recommendation.
- **Only include terms relevant for domain experts.** Skip the names of modules or classes unless they have meaning in the domain language.
- **Keep definitions tight.** One sentence max. Define what it IS, not what it does.
- **Show relationships.** Use bold term names and express cardinality where obvious.
- **Only include domain terms.** Skip generic programming concepts (array, function, endpoint) unless they have domain-specific meaning.
- **Note the absences.** A concept the humans talk about that has no representation in code is a finding — list it in "Code drift" with *(absent)*. So is a concept the code models that nobody has ever named out loud.
- **Group terms into multiple tables** when natural clusters emerge (e.g. by subdomain, lifecycle, or actor). Each group gets its own heading and table. If all terms belong to a single cohesive domain, one table is fine — don't force groupings.
- **Write an example dialogue.** A short conversation (3-5 exchanges) between a dev and a domain expert that demonstrates how the terms interact naturally. The dialogue should clarify boundaries between related concepts and show terms being used precisely.

<example>

## Example dialogue

> **Dev:** "How do I test the **sync service** without Docker?"

> **Domain expert:** "Provide the **filesystem layer** instead of the **Docker layer**. It implements the same **Sandbox service** interface but uses a local directory as the **sandbox**."

> **Dev:** "So **sync-in** still creates a **bundle** and unpacks it?"

> **Domain expert:** "Exactly. The **sync service** doesn't know which layer it's talking to. It calls `exec` and `copyIn` — the **filesystem layer** just runs those as local shell commands."

</example>

## Re-running

When invoked again in the same conversation:

1. Read the existing `UBIQUITOUS_LANGUAGE.md`
2. Incorporate any new terms from subsequent discussion
3. Re-scan the code, and re-check every `file:line` citation — stale citations are worse than
   missing ones. Drop or correct any that no longer resolve.
4. Update definitions if understanding has evolved
5. Re-flag any new ambiguities, and close out drift entries that have since been renamed
6. Rewrite the example dialogue to incorporate new terms

Preserve human edits. If a definition in the file has been rewritten by hand and the evidence
still supports it, keep their wording rather than reasserting your own.

---

<!--
LOCAL MODIFICATION — this file diverges from upstream (mattpocock/skills @ 697d4ce).
Added: codebase as a second source alongside the conversation ("Scope", "Reading a codebase",
the "In code" column, the "Code drift" section, and the related rules). Everything upstream
is otherwise intact. See ../README.md for provenance.
-->
