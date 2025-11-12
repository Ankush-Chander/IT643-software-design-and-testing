---
delivery date:
  - "[[2025-10-27]]"
---
### Quick recap
- Testing with external dependencies
- Test Doubles
	- mock, stub, spy, dummies, fake

---
### Agenda
- How to derive test cases?
---
### Types of Unit Test
1. **Specification tests**: Derive tests directly from the _specification_ of a function or class - that is, what it _should_ do, not how it’s implemented.   You don’t look at the code - you look at the **requirements**.
2. **Boundary tests**:  Boundaries are where most bugs hide — just below, at, and just above the limits of input ranges.
3. **Property-Based Testing**: In property-based testing, instead of coming up with concrete examples, we express the property that should hold for that method. The framework then randomly generates hundreds of different inputs.
4. **Structural Testing**: Design tests to ensure **every path**, **branch**, or **statement** in the code executes at least once.

---
### Specification based tests

![Specification based Testing](../images/testing/specification_nutshell.png)


---
#### Example: 
**Method**: substringsBetween()
Searches a string for substrings delimited by a start and end tag, returning all
matching substrings in an array.
-  str - The string containing the substrings. Null returns null; an empty
string returns another empty string.
-  open - The string identifying the start of the substring. An empty string
returns null.
-  close- The string identifying the end of the substring. An empty string
returns null.
The program returns a string array of substrings, or null if there is no match.

---
#### Code structure
Refer code: [here](../code/specification_testing)
```
.
├── src
│   ├── main.cpp
│   └── substrings_between.h
└── tests
    └── substring_between_test.cpp

3 directories, 3 files
```
---
#### **Steps for Specification-Based Testing**

1. **Understand the Requirements**
    - Grasp what the program should and shouldn’t do.
    - Identify input/output variables, their types, domains, and corner cases.
2. **Explore the Program**
    - Experiment with different inputs to understand its behavior.
    - Build a mental model of how the program works.
3. **Identify Input and Output Partitions**
    - Analyze each input’s type and valid range.
    - Study dependencies between variables.
    - Consider all possible output types and implicit business rules.
4. **Find Boundaries**
    - Focus on edge cases, as bugs often occur at partition boundaries.
5. **Design Test Cases**
    - Combine partitions and boundary conditions strategically.
    - Avoid unnecessary combinations by testing exceptional cases only once.
6. **Automate the Tests**
    - Write automated tests with clear inputs and expected outputs.
    - Keep test code clean, maintainable, and easy to debug.
7. **Refine with Creativity and Experience**
    - Review the test suite for gaps.
    - Add additional cases based on intuition or domain knowledge.
---
#### Specification-Based Testing in Practice

1. **Iterative, not sequential:**  
    The process of deriving test cases is **cyclic** — you often go back to refine partitions or boundaries while writing tests.
2. **Depth of testing depends on risk:**  
    Test **critical or high-risk areas** more thoroughly; be pragmatic where the cost of failure is low.
3. **Partition vs. boundary — don’t overthink it:**  
    It doesn’t matter whether a case is seen as a partition or a boundary — what matters is that it’s **tested**.
4. **On/off points are usually enough:**  
    Testing **boundary values** (“on” and “off” points) covers most cases, though adding extra “in/out” points can improve understanding.
5. **Use variations of a base input:**  
    Use a **common input seed** (like `"abc"`) and make small tweaks to derive multiple test cases — this simplifies analysis and debugging.
6. **Control combinatorial explosion:**
    - Reduce combinations by isolating exceptional cases.
    - Split large methods into smaller, testable units when combinations grow too large.
7. **Keep test inputs simple:**  
    Use the **simplest possible values** (small integers, short strings) unless complexity is necessary for coverage.
8. **Use realistic placeholders for irrelevant inputs:**  
    When certain inputs don’t affect behavior, use **reasonable, realistic defaults**.
9. **Test nulls and exceptions contextually:**  
    Only test null or edge cases **when they can realistically occur** in that component’s context (e.g., UI vs. internal logic).
10. **Use parameterized tests for repetitive cases:**  
    Avoid duplication by grouping similar tests using **parameterized frameworks**.
11. **Applies at all levels of granularity:**  
    The same partitioning and boundary reasoning applies to **methods, classes, and larger systems** (unit or integration tests).
12. **Include class state in OOP testing:**  
    When testing **stateful classes**, consider both **input parameters** and **object state** before invoking methods.
13. **Experience and creativity still matter:**  
    Systematic steps help, but experienced testers will design **better, more focused tests** based on intuition and domain insight.

---
#### Exercise
1. Implement adder : https://leetcode.com/problems/add-strings
2. Write specification based test cases for the same

---
## Boundary Testing
Boundaries are where most bugs hide — just below, at, and just above the limits of input ranges.

### Example

For a function:
> “`gradeFromMarks(int marks)` returns grade A/B/C/F based on:
> - 90–100 → A
> - 75–89 → B
> - 50–74 → C
> - <50 → F”

Test code:
```cpp
#include <string>
#include <cassert>

std::string gradeFromMarks(int marks) {
    if (marks >= 90) return "A";
    else if (marks >= 75) return "B";
    else if (marks >= 50) return "C";
    else return "F";
}

void test_grade_boundaries() {
    // Boundary values around grade cutoffs
    assert(gradeFromMarks(89) == "B"); // just below A
    assert(gradeFromMarks(90) == "A"); // at A boundary
    assert(gradeFromMarks(91) == "A"); // just above

    assert(gradeFromMarks(49) == "F"); // below pass
    assert(gradeFromMarks(50) == "C"); // at pass boundary
}

```

## Structural Testing
Design tests to ensure **every path**, **branch**, or **statement** in the code executes at least once.
### Example
Code to be testing
```cpp
double discount(double amount, bool isMember) {
    if (amount < 0) return 0;
    if (amount > 1000) return isMember ? 0.2 : 0.1;
    else return isMember ? 0.05 : 0.0;
}

```
Test code:
```cpp
#include <cassert>

void test_discount_structure() {
    // Path 1: Negative amount
    assert(discount(-10, false) == 0);
    // Path 2: amount > 1000 and member = true
    assert(discount(1500, true) == 0.2);
    // Path 3: amount > 1000 and member = false
    assert(discount(1500, false) == 0.1);
    // Path 4: amount <= 1000 and member = true
    assert(discount(800, true) == 0.05);
    // Path 5: amount <= 1000 and member = false
    assert(discount(800, false) == 0.0);
}

```

### Code coverage criteria
#### 1. Line Coverage
- **Goal:** Every line of code is executed at least once.
- **Example:** A single test that passes through an `if` statement counts as covered, even if only one branch is taken.
- **Limitation:** Doesn’t ensure all conditions or outcomes are tested.
---

#### 2. Branch Coverage
- **Goal:** Each branch of decision points (`if`, `for`, `while`, etc.) is executed at least once.
- **Example:** For `if (a && b)`, one test makes it true, another makes it false.
- **Measure:** 100% branch coverage means every _true/false edge_ in the control-flow graph (CFG) is covered. 
---

#### 3. Condition + Branch Coverage

- **Goal:** Each individual condition in a branch is evaluated both `true` and `false`, **and** each branch outcome (`true`/`false`) is covered.
- **Example:** For `if (A || B)`, testing A and B both true/false isn’t enough unless the whole `if` can also evaluate to false.
- **Benefit:** Prevents missing cases where the overall branch is always true or false.
---

#### 4. Path Coverage
- **Goal:** Every possible _execution path_ through the program is tested.
- **Strength:** Most rigorous and complete form of structural testing.
- **Limitation:** Exponential growth in paths (e.g., 3 conditions → 8 paths; 10 → 1024).  
    Loops make it practically impossible due to infinite or very large combinations.
---

## References
1. Chapter 2, Effective Software testing by MAURÍCIO ANICHE
