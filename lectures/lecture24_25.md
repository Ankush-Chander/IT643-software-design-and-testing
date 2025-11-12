---
delivery date:
---
## Quick recap
- How to derive test cases?
	- Specification testing
	- Boundary testing
	- Structural testing

---
## Testability and design 

- **Iceberg class**

| **RuleEvaluator**    |
| -------------------- |
| + Evaluate(String)   |
| - ParseAdjunctTerm() |
| - ParseFactorTerm()  |
| - ParseAdjunct()     |
| - HasMoreTokens()    |
| - GetNextToken()     |
Testability: Not good

---
### Fix 1
Make private function public.
Testable? Yes. Good design?

| **RuleEvaluator**    |
| -------------------- |
| + Evaluate(String)   |
| - ParseAdjunctTerm() |
| - ParseFactorTerm()  |
| - ParseAdjunct()     |
| + HasMoreTokens()    |
| - GetNextToken()     |

---
### Root cause
1. Code violates Single Responsibility Principle

| **RuleEvaluator**        |
| ------------------------ |
| ==+ Evaluate(String)==   |
| **- ParseAdjunctTerm()** |
| **- ParseFactorTerm()**  |
| **- ParseAdjunct()**     |
| *+ HasMoreTokens()*      |
| *- GetNextToken()*       |

---

| **RuleEvaluator**    |
| -------------------- |
| + Evaluate(String)   |
| - ParseAdjunctTerm() |
| - ParseFactorTerm()  |
| - ParseAdjunct()     |

| RuleTokenizer     |
| ----------------- |
| + HasMoreTokens() |
| + GetNextToken()  |

- The `RuleTokenizer` class now has a clean, public interface (`hasMoreTokens`, `getNextToken`), making it independently testable.
    - The original `RuleEvaluator` holds a private instance of the `RuleTokenizer`, maintaining its external encapsulation.
    - This new structure adheres to SRP, resulting in smaller, more focused, and more maintainable classes. The testability problem disappears as a side effect of improving the design.
---

### Conclusion
1. Non testability is a code smell that points us to bad design.
2. Forcing testability does not necessarily means improved design 
---

## Testing Pains and Their Corresponding Design Flaws

### 1. Difficult Setup

- **Testing Pain:** The process of instantiating a single class for a test becomes a complex detective-work job, requiring the creation of a cascade of dependent objects ("you find yourself kind of bringing half the application in just to instantiate one object").
- **Underlying Design Flaw:** **Excessive Coupling.** The class under test has too many dependencies and is not properly factored. Good design would break this gargantuan class into smaller, ==more cohesive units with fewer dependencies.==
---
### 2. State Leaks and Incomplete Shutdown

- **Testing Pain:** Tests fail intermittently or cause side effects in subsequent tests. This is often because a class does not release resources (e.g., file handles, network connections) or clean up static mutable data.
- **Underlying Design Flaw:** **Poor Resource Management and Use of Global State.** The class is not self-contained and fails to manage its own lifecycle. This often points to the ==use of Singletons or other forms of global mutable state, which testing makes "very real" and painful.==
---
### 3. Framework Frustration

- **Testing Pain:** Code is difficult to test because it is tightly interwoven with a framework (e.g., GUI, web, or persistence frameworks).
- **Underlying Design Flaw:** **Insufficient Separation of Concerns.** The core domain logic is not separated from the framework infrastructure. A better design separates these concerns, ==allowing the domain logic to be tested independently as a plain object, with the framework delegating calls to it.==
---
### 4. Difficult Mocking

- **Testing Pain:** The process of creating mock objects for dependencies is convoluted and painful.
- **Underlying Design Flaw:** This pain points to two distinct design violations:
    - **Violation of the Law of Demeter:** When tests require creating chains of mocks (a fake A returns a fake B, which returns a fake C), it signals that the production code has long dependency chains (`object.getA().getB().getC()`). This creates pervasive dependencies throughout the system.
    - **Violation of the Dependency Inversion Principle:** When it's hard to substitute a dependency (e.g., a component that talks to a third-party API), ==it's because the code depends on a concrete class rather than an abstraction (interface).== Introducing an interface makes substitution for testing straightforward and improves the design's flexibility.

---

### 5. Hidden Inputs and Effects

- **Testing Pain:** A class is untestable because its inputs or outputs are not exposed through its public interface. It may read from a hidden source (e.g., a global configuration) or write to an external system (e.g., a database) without any way to observe or control this behavior in a test.
- **Underlying Design Flaw:** **Encapsulation Violation and Insufficient Separation of Concerns.** ==A well-designed object should make its inputs and outputs clear. When side effects are hidden, the class is doing more than it claims.== The solution is to separate the computational logic from the external I/O logic into distinct classes.

---

### 6. Unwieldy Parameter Lists

- **Testing Pain:** Instantiating a class or calling a method is painful due to an excessive number of parameters.
- **Underlying Design Flaw:** **Too Many Responsibilities.** A long parameter list is a strong indicator that a class or method is trying to do too much. By writing tests, the developer becomes the "first client user of our code" and feels this pain concretely, motivating them to refactor the code into smaller, more focused units.
---

### 7. Test Thrash

- **Testing Pain:** A minor change in the production code causes a large number of tests to break.
- **Underlying Design Flaw:** **Violation of the Open/Closed Principle.** The design lacks good "locality of concerns," causing changes to ripple across the system. A well-designed system, often composed of small classes with single responsibilities, is more open to extension but closed to modification. ==Changes can be introduced by adding or substituting classes rather than modifying existing ones, minimising test breakage. ==

---
## Designing for Testability

### 1. Separate infrastructure code from domain code (at architectural level)

#### Bad design
```cpp
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

struct Invoice {
    std::string name;
    int value;
};

// Simulated database API (placeholder for actual DB calls)
class DatabaseConnection {
public:
    DatabaseConnection(const std::string& db, const std::string& user) {
        std::cout << "Connecting to DB: " << db << " as " << user << std::endl;
    }
    void close() {
        std::cout << "Closing DB connection" << std::endl;
    }

    std::vector<Invoice> executeQuery(const std::string& query) {
        std::cout << "Executing query: " << query << std::endl;
        // Pretend these come from the DB:
        return {
            {"CustomerA", 50},
            {"CustomerB", 120},
            {"CustomerC", 90}
        };
    }
};

class InvoiceFilter {
private:
    // Infrastructure logic embedded in domain class
    std::vector<Invoice> all() {
        try {
            DatabaseConnection conn("invoices_db", "root");
            std::string query = "SELECT * FROM invoices";
            std::vector<Invoice> allInvoices = conn.executeQuery(query);
            conn.close();
            return allInvoices;
        } catch (const std::exception& e) {
            std::cerr << "DB Error: " << e.what() << std::endl;
            return {};
        }
    }

public:
	
    // ❌ Domain logic mixed with infrastructure
    std::vector<Invoice> lowValueInvoices() {
        std::vector<Invoice> issuedInvoices = all();
        std::vector<Invoice> result;

        for (const auto& inv : issuedInvoices) {
            if (inv.value < 100)
                result.push_back(inv);
        }
        return result;
    }
};

int main() {
    InvoiceFilter filter;
    auto lowValue = filter.lowValueInvoices();

    std::cout << "Low-value invoices:\n";
    for (const auto& inv : lowValue)
        std::cout << " - " << inv.name << " ($" << inv.value << ")\n";
}

```
---
#### Known issues:
- Domain code and infrastructure code are mixed. This means we will not be able to avoid database access when testing the low-value invoices rule.
- The more responsibilities, the more complexity, and the more chances for bugs. Classes that are less cohesive contain more code.
---
#### Example
Suppose an online web shop has the following requirements:
For all the shopping carts that were paid today, the system should
-  Set the status of the shopping cart as ready for delivery, and persist its new
state in the database.
- Notify the delivery center, and let them know they should send the goods
to the customer.
-  Notify the SAP system.
-  Send an e-mail to the customer confirming that the payment was success-
ful. The e-mail should contain an estimate of when delivery will happen.
The information is available via the delivery center API.

---

#### Good Design
```c++
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <memory>

//-------------------------------------------
// Domain Model
//-------------------------------------------
class ShoppingCart {
    std::string customerName_;
    bool paid_;
    bool readyForDelivery_;
    std::chrono::system_clock::time_point estimatedDelivery_;
public:
    ShoppingCart(std::string customerName, bool paid)
        : customerName_(std::move(customerName)), paid_(paid), readyForDelivery_(false) {}

    const std::string& customerName() const { return customerName_; }
    bool isPaid() const { return paid_; }

    void markAsReadyForDelivery(std::chrono::system_clock::time_point day) {
        readyForDelivery_ = true;
        estimatedDelivery_ = day;
    }

    void printStatus() const {
        std::cout << "Cart for " << customerName_
                  << (readyForDelivery_ ? " is ready for delivery.\n"
                                        : " is not ready.\n");
    }
};

//-------------------------------------------
// Interfaces for collaborators (injected deps)
//-------------------------------------------

struct IShoppingCartRepository {
    virtual ~IShoppingCartRepository() = default;
    virtual std::vector<ShoppingCart> cartsPaidToday() = 0;
    virtual void persist(const ShoppingCart& cart) = 0;
};

struct IDeliveryCenter {
    virtual ~IDeliveryCenter() = default;
    virtual std::chrono::system_clock::time_point deliver(const ShoppingCart& cart) = 0;
};

struct ICustomerNotifier {
    virtual ~ICustomerNotifier() = default;
    virtual void sendEstimatedDeliveryNotification(const ShoppingCart& cart) = 0;
};

struct ISAP {
    virtual ~ISAP() = default;
    virtual void cartReadyForDelivery(const ShoppingCart& cart) = 0;
};

//-------------------------------------------
// Main class — PaidShoppingCartsBatch
//-------------------------------------------

class PaidShoppingCartsBatch {
    IShoppingCartRepository& db_;
    IDeliveryCenter& deliveryCenter_;
    ICustomerNotifier& notifier_;
    ISAP& sap_;
public:
    PaidShoppingCartsBatch(IShoppingCartRepository& db,
                           IDeliveryCenter& deliveryCenter,
                           ICustomerNotifier& notifier,
                           ISAP& sap)
        : db_(db), deliveryCenter_(deliveryCenter), notifier_(notifier), sap_(sap) {}

    void processAll() {
        auto paidCarts = db_.cartsPaidToday();

        for (auto& cart : paidCarts) {
            auto estimatedDay = deliveryCenter_.deliver(cart);
            cart.markAsReadyForDelivery(estimatedDay);
            db_.persist(cart);
            notifier_.sendEstimatedDeliveryNotification(cart);
            sap_.cartReadyForDelivery(cart);
        }
    }
};

```
---
![](images/testing/adaptors_ports.png)

---
### 2.  Dependency injection and Controllability
- Controllability
	- We should be able to control what a class under test does?
- Dependency Injection
	- All external dependencies should be passed into a class (e.g. via its constructor or setter methods) rather than hard-coded inside it.
	- By coding to abstractions (abstract classes, interfaces, or even C++ templates), unit tests can “plug in” stub or mock implementations.
---
### 3. Make classes and methods observable
Observability, at the class level, is about how easy it is to assert that the behavior of the functionality went as expected. 
Ensure that your classes provide developers with simple and easy ways to assert their state.
Common way:
- Introducing methods to facilitate assertions
- void methods are hard to test and can be improved by making them return some assertable value instead
---
### 4. Dependency via class constructor or value via method parameter
- Receiving a dependency via constructor adds a little complexity to the overall class and its tests but simplifies its client classes. 
- Receiving the data via method parameter simplifies the class and its tests but adds a little complexity to the clients. 

---
## References:
1. [Michael Feathers - the deep synergy between testability and good design - YouTube](https://youtu.be/4cVZvoFGJTU?list=TLGG0KwbRmmxQHswMzExMjAyNQ)
2. Chapter 7, Designing for Testability "Effective Software testing by MAURÍCIO ANICHE"
