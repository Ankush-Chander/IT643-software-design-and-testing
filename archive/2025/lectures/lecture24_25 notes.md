---
delivery date:
---

## TLDR
- Good design leads to testablity
- Testability of the system does not necessarily mean good design
- Common advice:
	- Separate infrastructure code from domain code (at architectural level)
		- Hexagon architecture/ Ports and adaptors
		- Example: 
			- Shopping cart example
			- Revisit snake example
	- Dependency injection and controllability
	- Make classes observable:
		- Observable(cant assert whether a method did it thing or not)
	- Dependency via class constructor or value via method parameter
- Designing for testability in Real world 

---


# Designing for Testability

Effective testable design means writing code that is **easy to test automatically**. As Aniche explains, “software systems are sometimes not ready for or designed to be tested,” so we should build in testability from the start[livebook.manning.com](https://livebook.manning.com/book/effective-software-testing/chapter-7#:~:text=Software%20systems%20are%20sometimes%20not,use%20to%20make%20testing%20easier). Key principles include **separating domain logic from infrastructure** and avoiding hidden dependencies. For example, database or network calls (infrastructure code) should be kept out of core business classes. Instead, core classes should operate against abstract interfaces or well-defined adapters – an approach embodied in the Hexagonal (Ports-and-Adapters) architecture[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=A%20practical%20tip%20at%20the,party%20services%2C%20etc). This separation lets tests focus on the business logic in isolation (for instance, swapping in a fake database or mock service during testing).

 

Similarly, Aniche emphasizes **dependency injection and controllability**: all external dependencies should be passed into a class (e.g. via its constructor or setter methods) rather than hard-coded inside it[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor). By coding to abstractions (abstract classes, interfaces, or even C++ templates), unit tests can “plug in” stub or mock implementations. As one author summarizes, applying the Dependency Inversion Principle and passing dependencies through constructors leads to “significantly fewer testing problems”[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor). In practice, this means designing every class so that its collaborators are replaceable. For instance, instead of doing `MyClass m;` inside a function, do `MyClass m(myDependency);` where `myDependency` is an interface or templated type provided by the caller.

 

Another core idea is **observability**: tests must be able to observe and assert on the behavior of the code under test[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor). If a method under test produces output or changes internal state, it should expose that outcome in a testable way (e.g. via return values, getters, or injected callbacks) rather than hiding it in side effects. Aniche shows examples of _adding methods or parameters_ purely to make assertions possible. In C++, this often means avoiding direct use of `std::cout` or globals – instead inject an `ostream&` or logger so tests can capture output. Romanov likewise advises that a test “must be able to see the state of critical parts of the object” and suggests adding explicit methods to expose that state[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor).

 

Finally, maintain **high cohesion and low coupling**: each class should do one thing well, and depend on as little as possible. Aniche and Fowler both note that tightly-coupled or monolithic code is hard to test[martinfowler.com](https://martinfowler.com/articles/testing-culture.html#:~:text=must%20be%20repaid%20later,it%20must%20be%20paid%20to)[martinfowler.com](https://martinfowler.com/articles/testing-culture.html#:~:text=The%20real%20magic%20happens%20when,the%20ones%20that%20still%20do). Designing for testability will inherently produce code that is easier to review, maintain, and extend[martinfowler.com](https://martinfowler.com/articles/testing-culture.html#:~:text=The%20real%20magic%20happens%20when,extend%2C%20debug%2C%20analyze%20with%20other). For example, avoid sprawling classes that mix many concerns; instead factor complex logic into small helper functions or classes. Likewise, steer clear of **singletons, global state or static methods** that secretly carry dependencies – as one C++ expert warns, “an overuse of global variables and singleton pattern (anti-pattern) creates dependency entanglement” that is “usually not unit-testable”[bulldogjob.com](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=On%20a%20final%20note%2C%20a,testable%20anyway%20and%20is%20the).

## C++ Examples: Dependency Injection and Observability

We can apply these ideas in C++ with concrete patterns:

- **Constructor or Setter Injection:** Pass collaborator objects into a class. For example, instead of hard-coding a database instance, take it through the constructor. In the code below, `DataProcessor` depends on a `Database` interface; in production we pass the real database, while tests can pass a mock.
    
    `struct IDatabase {   virtual ~IDatabase() = default;   virtual void write(const std::string& data) = 0; };  class RealDatabase : public IDatabase {   void write(const std::string& data) override {     // Actual database write...   } };  class DataProcessor {   std::shared_ptr<IDatabase> db_; public:   DataProcessor(std::shared_ptr<IDatabase> db) : db_(std::move(db)) {}   void processData(const std::string& input) {     // Business logic, then store result:     std::string result = "[" + input + "]";     db_->write(result);   } };`
    
    In a unit test we can inject a fake database to _observe_ what was written:
    
    `class MockDatabase : public IDatabase { public:   std::vector<std::string> written;   void write(const std::string& data) override {     written.push_back(data);   } };  // In test: auto mockDb = std::make_shared<MockDatabase>(); DataProcessor processor(mockDb); processor.processData("test"); assert(mockDb->written.size() == 1); assert(mockDb->written[0] == "[test]");`
    
    By using interfaces and injection, the class is fully testable with no I/O or globals, enabling fast, deterministic tests[bulldogjob.com](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=In%20order%20to%20be%20able,DI)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor).
    
- **Template-based Injection:** C++ templates offer an alternative to runtime polymorphism. Instead of using an abstract base class, you can template the dependency. The example below shows a free function where the dependent object type is a template parameter, avoiding virtual calls entirely[stackoverflow.com](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided). This can be more efficient and still easily mockable by passing in a stub type.
    
    `template <typename LogPolicy> void processAndLog(const std::string& data, LogPolicy& logger) {     // Process data (pure logic):     std::string result = "Processed(" + data + ")";     // Log or output:     logger.log(result); }  // A real logger and a fake logger: struct ConsoleLogger {   void log(const std::string& msg) { std::cout << msg; } }; struct StringLogger {   std::string text;   void log(const std::string& msg) { text = msg; } };  // In test: StringLogger fake; processAndLog("value", fake); assert(fake.text == "Processed(value)");`
    
    Here `processAndLog` does not depend on a specific logging class – any type with a `log(std::string)` method will work. A mock logger simply records the string, allowing assertions. This template technique sidesteps virtual dispatch overhead while achieving the same testability benefit as injecting an interface[stackoverflow.com](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided).
    
- **Observability Through Outputs or Getters:** Design your classes so tests can observe their results. For example, a class might accumulate some state internally; tests need a way to check that state. Below, `Counter` adds a getter to expose its value. Without `getCount()`, its behavior would be hidden and hard to test.
    
    `class Counter {   int count_ = 0; public:   void increment() { ++count_; }   int getCount() const { return count_; }  // Observability for tests };  // In test: Counter c; c.increment(); c.increment(); assert(c.getCount() == 2);`
    
    Similarly, prefer to return data rather than print it. If you have a function that “does something” but returns void, consider refactoring so its core logic returns a value. For instance, instead of:
    
    `void greet(const std::string& name) {     std::cout << "Hello " << name; }`
    
    write a function that returns a `std::string`, or inject an `ostream&`:
    
    `std::string makeGreeting(const std::string& name) {     return "Hello " + name; } // or void greet(const std::string& name, std::ostream& out) {     out << "Hello " << name; }`
    
    In tests you can then use a `std::ostringstream` to capture the output and assert on it. Exposing outputs and state in this way adheres to Aniche’s advice about making classes **observable for assertions**[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor).
    

## Design Patterns and Pitfalls in C++

Beyond the book, C++ developers follow several idioms that align with these principles:

- **Dependency Inversion & Injection:** Always code to abstract interfaces (abstract base classes or concepts) and inject dependencies rather than constructing them internally[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[bulldogjob.com](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=In%20order%20to%20be%20able,DI). For example, use constructor injection or method injection to decouple classes. The Service Locator pattern should be avoided in favor of explicit injection. This matches the core SOLID principles: e.g. classes depend on interfaces (DIP), which in C++ can be pure virtual base classes, or template parameters for static polymorphism[stackoverflow.com](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided).
    
- **Ports and Adapters (Hexagonal Architecture):** Define “ports” (interfaces) for each external interaction and write “adapters” that implement them. This ensures business logic never directly touches, say, `std::filesystem`, `std::cout`, or network APIs. In practice, this might mean wrapping external calls in small adapter classes and injecting those into your core classes[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=A%20practical%20tip%20at%20the,party%20services%2C%20etc).
    
- **Factory/Builder Patterns for Construction:** When objects have complex setup, use factory or builder classes that can be overridden in tests. For instance, a factory that produces a `Logger` could be replaced with one that returns a `MockLogger`. This keeps creation logic out of the domain classes themselves.
    
- **Avoid Singletons and Globals:** As noted above, singletons and global variables create hidden dependencies that break isolation[bulldogjob.com](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=On%20a%20final%20note%2C%20a,testable%20anyway%20and%20is%20the). In C++, static storage (e.g. non-const globals or Meyers’ singletons) should be minimized. If necessary, restrict them to startup code; all other code should explicitly receive its collaborators. Static methods that access static state are similarly risky for testability.
    
- **Use of Templates vs Virtuals:** C++ offers templates as a way to inject behavior at compile time (as shown above). This avoids the performance and maintenance cost of a profusion of virtual methods. A balance is usually to make only the boundaries virtual. In fact, one C++ practitioner notes that you can achieve the same injection without any interface by templating the function or class[stackoverflow.com](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided). This “generic programming” approach can reduce boilerplate of mock classes, at the cost of greater coupling at compile time.
    
- **Mocking and Test Frameworks:** Although not a design pattern per se, using a mocking framework like GoogleMock is common. To use it, classes under test should have virtual methods (or use interfaces) so the framework can override behavior. GoogleMock exploits dependency injection and polymorphism, so the above patterns enable its use. In modern C++ you may also use constexpr, concepts or newer libraries, but the core idea remains: structure code so behaviors can be replaced by test doubles.
    
- **Pitfalls to Avoid:** Tight coupling and low cohesion are the enemies of testability. If a class does many unrelated things, it’ll be hard to test independently – refactor it into smaller pieces. Beware of “private method” issues: private helper functions should either be so trivial they need no direct testing, or be refactored into public methods of collaborator objects. Also, don’t overuse exceptions for flow control in a way that makes tests cumbersome. Lastly, overly clever code (“cleverness tax”) can hurt test clarity; simple, clear logic is easier to reason about in tests.
    

Overall, a key takeaway is that **design for testability often coincides with good design**: high cohesion, loose coupling, clear interfaces, and small classes[martinfowler.com](https://martinfowler.com/articles/testing-culture.html#:~:text=must%20be%20repaid%20later,it%20must%20be%20paid%20to)[martinfowler.com](https://martinfowler.com/articles/testing-culture.html#:~:text=The%20real%20magic%20happens%20when,extend%2C%20debug%2C%20analyze%20with%20other). By applying these principles in C++ – via dependency injection (using interfaces or templates), isolating side-effects, and exposing observability – developers create code that is not only well-tested but also easier to maintain and evolve.

 

**Sources:** Summarized and adapted from _Effective Software Testing_ (Aniche)[livebook.manning.com](https://livebook.manning.com/book/effective-software-testing/chapter-7#:~:text=Software%20systems%20are%20sometimes%20not,use%20to%20make%20testing%20easier)[henrikwarne.com](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this), supplemented with industry best practices for C++ (e.g. dependency injection patterns[bulldogjob.com](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=In%20order%20to%20be%20able,DI)[stackoverflow.com](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided) and architecture-level guidance[alexromanov.github.io](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=A%20practical%20tip%20at%20the,party%20services%2C%20etc)).

Citations

[

![](https://www.google.com/s2/favicons?domain=https://livebook.manning.com&sz=32)

7 Designing for testability · Effective Software Testing: A developer's guide

https://livebook.manning.com/book/effective-software-testing/chapter-7

](https://livebook.manning.com/book/effective-software-testing/chapter-7#:~:text=Software%20systems%20are%20sometimes%20not,use%20to%20make%20testing%20easier)[

![](https://www.google.com/s2/favicons?domain=https://henrikwarne.com&sz=32)

Effective Software Testing – A Developer’s Guide | Henrik Warne's blog

https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/

](https://henrikwarne.com/2022/06/19/effective-software-testing-a-developers-guide/#:~:text=Designing%20for%20testability,all%20in%20favor%20of%20this)[

![](https://www.google.com/s2/favicons?domain=https://alexromanov.github.io&sz=32)

Testability and how to improve it - Test Engineering Notes

https://alexromanov.github.io/2022/08/14/what-is-testability/

](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=A%20practical%20tip%20at%20the,party%20services%2C%20etc)[

![](https://www.google.com/s2/favicons?domain=https://alexromanov.github.io&sz=32)

Testability and how to improve it - Test Engineering Notes

https://alexromanov.github.io/2022/08/14/what-is-testability/

](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor)[

![](https://www.google.com/s2/favicons?domain=https://alexromanov.github.io&sz=32)

Testability and how to improve it - Test Engineering Notes

https://alexromanov.github.io/2022/08/14/what-is-testability/

](https://alexromanov.github.io/2022/08/14/what-is-testability/#:~:text=If%20the%20developer%20applies%20the,dependencies%20through%20the%20class%20constructor)[

![](https://www.google.com/s2/favicons?domain=https://martinfowler.com&sz=32)

Goto Fail, Heartbleed, and Unit Testing Culture

https://martinfowler.com/articles/testing-culture.html

](https://martinfowler.com/articles/testing-culture.html#:~:text=must%20be%20repaid%20later,it%20must%20be%20paid%20to)[

![](https://www.google.com/s2/favicons?domain=https://martinfowler.com&sz=32)

Goto Fail, Heartbleed, and Unit Testing Culture

https://martinfowler.com/articles/testing-culture.html

](https://martinfowler.com/articles/testing-culture.html#:~:text=The%20real%20magic%20happens%20when,the%20ones%20that%20still%20do)[

![](https://www.google.com/s2/favicons?domain=https://martinfowler.com&sz=32)

Goto Fail, Heartbleed, and Unit Testing Culture

https://martinfowler.com/articles/testing-culture.html

](https://martinfowler.com/articles/testing-culture.html#:~:text=The%20real%20magic%20happens%20when,extend%2C%20debug%2C%20analyze%20with%20other)[

![](https://www.google.com/s2/favicons?domain=https://bulldogjob.com&sz=32)

On Dependency Injection, Loose Coupling and Unit Tests in C++

https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c

](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=On%20a%20final%20note%2C%20a,testable%20anyway%20and%20is%20the)[

![](https://www.google.com/s2/favicons?domain=https://bulldogjob.com&sz=32)

On Dependency Injection, Loose Coupling and Unit Tests in C++

https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c

](https://bulldogjob.com/readme/on-dependency-injection-loose-coupling-and-unit-tests-in-c#:~:text=In%20order%20to%20be%20able,DI)[

![](https://www.google.com/s2/favicons?domain=https://stackoverflow.com&sz=32)

testing - Patterns for making c++ code easy to test - Stack Overflow

https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test

](https://stackoverflow.com/questions/1680971/patterns-for-making-c-code-easy-to-test#:~:text=A%20workaround%20I%20often%20use,of%20virtual%20functions%20is%20avoided)

All Sources