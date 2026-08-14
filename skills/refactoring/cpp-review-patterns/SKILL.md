---
name: cpp-review-patterns
description: Use when reviewing C++ code — covers memory management and RAII, undefined behavior traps, template pitfalls, const correctness, move semantics, and common anti-patterns. C++ demands a specialized review lens because manual memory, implicit conversions, and template metaprogramming create failure modes invisible to reviewers trained primarily on GC languages. Load alongside review-accuracy-calibration to calibrate confidence before posting findings.
---

# C++ Code Review Patterns

## Overview

C++ combines the expressive power of high-level abstractions with direct control over hardware resources — and pays for it with a class of bugs that no compiler or runtime can fully prevent. A reviewer trained on Java, Python, or even Rust will miss the failure modes that are unique to C++: resource leaks that appear only under exceptions, undefined behavior that silently produces wrong results in optimized builds, template errors that require a compiler archaeology dig to interpret, and move semantics that invalidate objects in ways that look valid at a glance.

This guide covers six areas where C++ code most commonly fails in review or in production: memory management and RAII, undefined behavior, template pitfalls, const correctness, move semantics, and systemic anti-patterns. Each section includes before/after code examples and severity rules calibrated to real-world impact.

Load this skill when reviewing any C++ PR that touches resource acquisition, performance-critical hot paths, public APIs, or code that must be exception-safe. Cross-reference `review-accuracy-calibration` before posting: many C++ issues are compiler-detectable with the right flags (`-Wall -Wextra -fsanitize=address,undefined`), and findings backed by sanitizer evidence are C4; findings based on code reading alone are typically C3.

## Quick Reference

| Review Dimension | Severity | Primary Red Flag |
|---|---|---|
| Raw new/delete without RAII | HIGH | `new` not paired with a smart pointer or RAII wrapper |
| Missing custom destructor | HIGH | Class owns a raw pointer but has no destructor, or has destructor but no copy/move ops |
| Shared ownership overuse | MEDIUM | `shared_ptr` where `unique_ptr` suffices; creates circular reference risk |
| Signed integer overflow | CRITICAL | Arithmetic on `int` with no overflow guard; UB in optimized builds |
| Use-after-free / dangling reference | CRITICAL | Pointer or reference to a destroyed object or invalidated container element |
| Uninitialized variable | HIGH | POD type declared without initializer; value is indeterminate |
| Strict aliasing violation | HIGH | `reinterpret_cast` between unrelated pointer types without `memcpy` |
| Template error message opacity | LOW–MEDIUM | Deep template instantiation with no `static_assert` guard |
| Missing `concept` constraint | MEDIUM | C++20 template accepting any type where only a subset is valid |
| Non-const method on logically const object | MEDIUM | Method mutates only cached state without `mutable`; blocks use in const contexts |
| `std::move` on const object | HIGH | `std::move(const T)` silently falls back to copy; intent unclear |
| Missing `noexcept` on move constructor | MEDIUM | Move constructor not `noexcept`; `std::vector` reallocation copies instead of moves |
| C-style cast | MEDIUM–HIGH | `(T*)ptr` hides the cast category; prefer `static_cast`, `reinterpret_cast`, or `bit_cast` |
| Exception in destructor | CRITICAL | Throwing from a destructor during stack unwinding calls `std::terminate` |
| Macro with side-effect argument | HIGH | Macro argument evaluated multiple times; use `inline` function or `constexpr` |

## Memory Management and RAII

RAII (Resource Acquisition Is Initialization) is the primary C++ idiom for correct resource lifetime management. Any resource acquired in a constructor must be released in the destructor; smart pointers automate this for heap memory. Reviews should flag any deviation from this contract.

**Raw new/delete — before:**

```cpp
void process_data(size_t n) {
    int* buffer = new int[n];    // Leak if an exception is thrown below.
    do_work(buffer, n);          // If do_work throws, destructor never runs.
    delete[] buffer;             // Unreachable on exception path.
}
```

**After — RAII via unique_ptr:**

```cpp
void process_data(size_t n) {
    auto buffer = std::make_unique<int[]>(n);  // Freed automatically on any exit.
    do_work(buffer.get(), n);
}
```

**Shared ownership — use `shared_ptr` only when ownership is genuinely shared:**

```cpp
// WRONG: shared_ptr where a single owner exists; misleads readers, adds overhead.
std::shared_ptr<Config> cfg = std::make_shared<Config>(path);

// CORRECT: unique ownership expressed clearly.
std::unique_ptr<Config> cfg = std::make_unique<Config>(path);

// CORRECT: shared ownership when multiple subsystems outlive the creator.
std::shared_ptr<Logger> logger = std::make_shared<Logger>();
subsystem_a.set_logger(logger);
subsystem_b.set_logger(logger);
```

**Rule of Five** — if a class declares a destructor, copy constructor, copy assignment, move constructor, or move assignment, it almost certainly needs all five. A class with a custom destructor that omits copy/move operations will generate implicit shallow copies of raw pointers.

**Circular `shared_ptr` references** — two objects holding `shared_ptr` to each other will never be destroyed. Flag any object graph where `shared_ptr` could form a cycle; the fix is `weak_ptr` for back-edges.

**Severity:** Raw `new` without a corresponding smart pointer at the same scope: HIGH. `shared_ptr` where `unique_ptr` would suffice: MEDIUM. Missing Rule of Five members when one is user-defined: HIGH. Circular `shared_ptr` cycle with no `weak_ptr` break: HIGH.

## Undefined Behavior

Undefined behavior (UB) in C++ is not a runtime exception — it is the compiler's license to assume the UB path is never taken, enabling transformations that silently produce wrong or dangerous output in release builds. Sanitizers (`-fsanitize=undefined,address`) catch UB at runtime; static analysis and careful review catch it before it ships.

**Signed integer overflow — before:**

```cpp
int count_bytes(int n) {
    return n * 1024;  // If n > 2,097,151, the multiplication overflows — UB.
                      // Optimizer may assume n <= 2,097,151 and eliminate bounds checks.
}
```

**After — use unsigned or wider types where overflow is possible:**

```cpp
size_t count_bytes(size_t n) {
    return n * 1024UZ;  // size_t is unsigned; overflow wraps (defined behavior).
}
// Or: check before multiplying.
int count_bytes_checked(int n) {
    assert(n <= INT_MAX / 1024);
    return n * 1024;
}
```

**Dangling reference — before:**

```cpp
const std::string& get_greeting() {
    std::string msg = "hello";
    return msg;  // Returns reference to a local variable; UB on access.
}
```

**After — return by value:**

```cpp
std::string get_greeting() {
    return "hello";  // Move or NRVO eliminates the copy in practice.
}
```

**Iterator invalidation** — inserting into or erasing from a `std::vector` invalidates all iterators and pointers into that vector. Storing a pointer or iterator to a `vector` element across a mutation is UB.

```cpp
std::vector<int> v = {1, 2, 3};
int* p = &v[0];     // Valid now.
v.push_back(4);     // Reallocation may occur; p is now dangling.
*p = 99;            // UB: p may point to freed memory.
```

**Strict aliasing** — the compiler assumes pointers of different types do not alias the same memory. Casting between unrelated pointer types and then dereferencing violates this rule.

```cpp
// WRONG: reinterpret_cast between float* and int* violates strict aliasing.
float f = 3.14f;
int bits = *reinterpret_cast<int*>(&f);   // UB.

// CORRECT: use memcpy or std::bit_cast (C++20).
int bits;
std::memcpy(&bits, &f, sizeof(bits));     // Defined; compilers optimize to register move.
// Or: int bits = std::bit_cast<int>(f);  // C++20, type-safe.
```

**Severity:** Signed overflow in arithmetic on user-controlled input: CRITICAL. Dangling reference or pointer returned from a function: CRITICAL. Iterator invalidation after container mutation: HIGH. Strict aliasing violation via `reinterpret_cast`: HIGH.

## Template Pitfalls

C++ templates provide zero-cost abstraction but generate error messages that span hundreds of lines and make incorrect usage hard to diagnose. Review should verify that template interfaces are constrained, instantiation errors are guarded by `static_assert`, and header inclusion cost is justified.

**Unconstrained template — before:**

```cpp
template <typename T>
void serialize(T value) {
    value.write(stream_);  // If T lacks write(), the error appears deep in instantiation.
}
```

**After — use a concept constraint (C++20):**

```cpp
template <typename T>
concept Serializable = requires(T v, Stream& s) { v.write(s); };

template <Serializable T>
void serialize(T value) {
    value.write(stream_);
}
// Pre-C++20: use static_assert with type traits.
template <typename T>
void serialize(T value) {
    static_assert(has_write_v<T>, "T must provide a write(Stream&) method");
    value.write(stream_);
}
```

**SFINAE complexity** — substitution failure is not an error, but deeply nested SFINAE via `std::enable_if` is hard to read and harder to debug. Flag SFINAE chains that can be replaced by `if constexpr` (C++17) or concepts (C++20): MEDIUM.

```cpp
// BEFORE: SFINAE enable_if chain.
template <typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
T double_value(T x) { return x * 2; }

// AFTER: if constexpr or concept.
template <typename T>
T double_value(T x) {
    static_assert(std::is_integral_v<T>, "double_value requires an integral type");
    return x * 2;
}
```

**Two-phase lookup** — in a template body, names that depend on a template parameter are looked up at instantiation time; names that do not are looked up at definition time. A call to a base class method inside a derived template class is not found unless qualified with `this->` or the base class name.

```cpp
template <typename T>
struct Derived : Base<T> {
    void action() {
        helper();        // WRONG: helper is a dependent name; not found at definition time.
        this->helper();  // CORRECT: defers lookup to instantiation.
    }
};
```

**Header bloat** — heavy template instantiations in headers (e.g., including `<algorithm>` and `<regex>` in a widely-included utility header) increase compile times across the entire project. Flag template implementations in widely-included headers that could be moved to a `.cpp` with explicit instantiation: MEDIUM.

**Severity:** Unconstrained template with a type-error cliff: MEDIUM. Two-phase lookup violation: HIGH (compile error in correct compilers, silent failure in others). SFINAE chain replaceable by concepts: MEDIUM. Heavy template header pulled into a low-level utility: MEDIUM.

## Const Correctness

`const` in C++ is a promise: a `const`-qualified method will not modify observable object state, and a `const` reference will not be used to mutate the referent. Violations erode this contract and force callers to use unnecessary non-const references.

**Non-const method on logically const operation — before:**

```cpp
class Cache {
public:
    std::string get(const std::string& key) {  // Should be const; does not modify state.
        return data_[key];                      // operator[] on map inserts default — a mutation!
    }
private:
    std::map<std::string, std::string> data_;
};
```

**After — use `find` in const methods; use `mutable` for cache fields:**

```cpp
class Cache {
public:
    std::optional<std::string> get(const std::string& key) const {
        auto it = data_.find(key);
        return it != data_.end() ? std::optional{it->second} : std::nullopt;
    }
    // For genuinely cached computation:
    int expensive_hash() const {
        if (!hash_cache_) hash_cache_ = compute_hash();
        return *hash_cache_;
    }
private:
    std::map<std::string, std::string> data_;
    mutable std::optional<int> hash_cache_;  // mutable: logical const, physical mutation.
};
```

**Const reference vs value** — accepting a large object by `const T&` avoids copies; accepting by value allows moves when the caller is done with the object. Review that function signatures choose the appropriate form.

```cpp
// Prefer const reference for read-only access to large objects.
void log_event(const Event& e);

// Prefer by-value when the function stores or transforms the argument.
void enqueue(Task task);  // Caller can std::move in; function owns the copy.
```

**Const propagation through pointers** — `const T* p` is a pointer to const T (cannot modify T through p); `T* const p` is a const pointer to T (cannot reseat p). Reviewers should verify the intent matches the declaration.

**Severity:** Non-const method that performs no state mutation: MEDIUM (blocks use in const contexts). `const_cast` to remove `const` on a non-`mutable` member: HIGH (UB if the object was originally declared `const`). Missing `const` on a parameter passed by reference when the function does not mutate it: LOW.

## Move Semantics

Move semantics allow C++ to transfer ownership of resources instead of copying them. Incorrect use — especially `std::move` on a const object or a moved-from object — silently degrades to copies or produces objects in an indeterminate state.

**std::move on const object — before:**

```cpp
class Pipeline {
public:
    void add_stage(const Stage& stage) {
        stages_.push_back(std::move(stage));  // std::move on const: no-op move.
                                               // Overload resolution selects copy constructor.
    }
private:
    std::vector<Stage> stages_;
};
```

**After — accept by value to enable both copy and move:**

```cpp
class Pipeline {
public:
    void add_stage(Stage stage) {                   // Caller decides: copy or move.
        stages_.push_back(std::move(stage));        // Now moves from a non-const lvalue.
    }
private:
    std::vector<Stage> stages_;
};
// Call sites:
pipeline.add_stage(stage);             // Copies.
pipeline.add_stage(std::move(stage));  // Moves; caller's stage is in moved-from state.
```

**Moved-from state** — after `std::move`, the source object is in a valid but unspecified state. Subsequent use of the moved-from object is legal only if the operation does not assume a particular value (e.g., `clear()`, reassignment, or destruction). Flag any code that reads a moved-from object without first reinitializing it.

**Rule of Five for move-aware classes:**

```cpp
class Resource {
public:
    explicit Resource(size_t n) : data_(new int[n]), size_(n) {}

    // Rule of Five: destructor + copy + move.
    ~Resource() { delete[] data_; }

    Resource(const Resource& other) : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }
    Resource& operator=(const Resource& other) {
        if (this != &other) {
            Resource tmp(other);
            std::swap(data_, tmp.data_);
            std::swap(size_, tmp.size_);
        }
        return *this;
    }
    Resource(Resource&& other) noexcept : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;  // Leave moved-from in valid state.
        other.size_ = 0;
    }
    Resource& operator=(Resource&& other) noexcept {
        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

private:
    int* data_;
    size_t size_;
};
```

**noexcept on move constructors** — `std::vector` and other standard containers will copy rather than move elements during reallocation if the move constructor is not marked `noexcept`. Flag move constructors that are not `noexcept` on types expected to be stored in vectors: MEDIUM.

**Severity:** `std::move` on a const object: HIGH (silent copy; intent violated). Use of moved-from object without reinitialization: HIGH. Missing `noexcept` on move constructor for a value type: MEDIUM. Missing Rule of Five when one special member is user-defined: HIGH.

## Anti-Patterns

**C-style casts** — `(T*)ptr` can silently perform a `reinterpret_cast`, a `static_cast`, a `const_cast`, or a combination. Named casts make intent explicit and are easier to search for in code review.

```cpp
// WRONG: C-style cast obscures what conversion is happening.
int* p = (int*)raw_ptr;

// CORRECT: named cast documents the category.
int* p = static_cast<int*>(raw_ptr);       // Compile-time checked conversion.
int* p = reinterpret_cast<int*>(raw_ptr);  // Explicit: bit-pattern reinterpretation.
```

**Macro abuse** — function-like macros evaluate arguments multiple times and ignore scope. Prefer `inline` functions, `constexpr`, or templates.

```cpp
// WRONG: MAX(a++, b) increments a twice if a > b initially.
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// CORRECT: template function with a single evaluation.
template <typename T>
constexpr T max_val(T a, T b) { return a > b ? a : b; }
```

**Global mutable state** — non-const global and `static` variables are initialized in an unspecified order across translation units (static initialization order fiasco) and create implicit dependencies between unrelated components. Flag global mutable state in libraries: HIGH. In application code, prefer dependency injection or the singleton pattern with explicit initialization order.

**Exceptions in destructors** — if a destructor throws during stack unwinding caused by another exception, `std::terminate` is called. All destructors must either handle exceptions internally or be marked `noexcept`.

```cpp
// WRONG: destructor may throw.
~FileHandle() {
    file_.close();  // close() may throw on flush failure.
}

// CORRECT: swallow or log the exception in the destructor.
~FileHandle() noexcept {
    try { file_.close(); }
    catch (...) { /* log, ignore; cannot propagate */ }
}
```

**`printf`-style format strings with C++ types** — mixing `printf` with `std::string` or other non-POD types is UB. Flag any `printf`/`sprintf`/`fprintf` call where a non-POD argument is passed: CRITICAL. Use `std::format` (C++20), `fmtlib`, or `std::cout`.

## Calibration Notes

Apply the C4/C3/C2/C1 confidence model from `review-accuracy-calibration` before posting any finding.

- **Memory and RAII issues (C4 with sanitizer evidence):** A raw `new` without a corresponding RAII wrapper is always a finding. If an AddressSanitizer report accompanies the PR, the finding is C4. Without dynamic evidence, use C3 for use-after-free suspicions based on code reading alone.
- **Undefined behavior (C3–C4):** Signed overflow and dangling references are C4 when the code path is clearly reachable. UB that requires aliasing analysis or optimizer knowledge (strict aliasing) is C3 — note that the compiler is permitted to exploit it but may not in current build settings.
- **Template errors (C2–C3):** Unconstrained templates and two-phase lookup issues are C3 when you can construct a failing instantiation. Do not flag C1 template concerns without a concrete counterexample type.
- **Const correctness (C3–C4):** Missing `const` on a method that provably does not mutate state: C4. `const_cast` correctness depends on the original declaration of the object — verify before flagging C4.
- **Move semantics (C3–C4):** `std::move` on a const object is C4 (compiler evidence: overload resolution selects copy). Missing `noexcept` on a move constructor is C3 — verify the type is actually stored in `std::vector` or other containers that check `is_nothrow_move_constructible`.

## Cross-References

- `review-accuracy-calibration` — Apply C4/C3/C2/C1 confidence scoring before posting any C++ finding; use the calibration table to distinguish high-confidence from speculative comments
- `rust-review-patterns` — Rust's ownership model is the compiler-enforced analog of C++ RAII and smart pointers; comparing the two clarifies why C++ requires manual discipline where Rust enforces by construction
- `concurrency-patterns` — C++ `std::mutex`, `std::atomic`, and `std::condition_variable` patterns; lock ordering, spurious wakeups, and data race detection via ThreadSanitizer
- `error-handling-patterns` — C++ exception safety guarantees (basic, strong, no-throw) and the relationship between exception specifications and resource management
