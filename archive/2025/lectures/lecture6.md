---
delivery date:
  - "[[2025-08-04]]"
---
#### Quick recap
- UML (Unified Modelling Language)
	- Types of UML Diagrams
---
#### Agenda 
- Design principles
	- Composition over inheritance
	- Encapsulate What Varies
	- Program Against Abstractions, Not Concretions  
	- The Hollywood Principle: "Don't Call Us, We'll Call You"
---
##### Composition Over Inheritance
Favour **object composition** (has-a relationships) rather than **class inheritance** (is-a relationships) to achieve code reuse and flexibility.  
- **Why:**
	- Inheritance creates tight coupling and rigid hierarchies.
	- Composition allows dynamic behavior changes at runtime.
---
###### Example
- **Inheritance (is-a)**: A `Car` _is a_ `Vehicle`. This is a good use case for inheritance. However, creating subclasses for every variation (e.g., `PetrolCar`, `ElectricCar`) can lead to a class explosion.
    
- **Composition (has-a)**: A `Car` _has an_ `Engine`. This is more flexible. The `Car` object can contain a pointer to an `Engine` object. You can easily switch between a `PetrolEngine` and an `ElectricEngine` without changing the `Car` class, even at runtime.
---
```c++
class Engine {
public:
    void start() { /* ... */ }
};

class Car {
private:
    Engine engine;  // Composition: Car has-an Engine
public:
    void start() { engine.start(); }
};

```
instead of 
```c++
class Engine {
public:
    void start() { /* ... */ }
};

class Car : public Engine {
// Inheritance: Car is-an Engine (makes no real-world sense)
};

```
---
##### Encapsulate What Varies
Identify the parts of your code that **change frequently** and **encapsulate** them, so changes don't affect the rest of the system.

**Why:**
- Keeps the stable parts of the system insulated from frequent changes.
- Encourages modular design.

---
###### Example:
Imagine a video game where characters can have different attack behaviors. The attack logic is something that might vary (sword, bow, magic). We should encapsulate this behavior.

1. **Identify what varies**: The attack behavior.
2. **Encapsulate it**: Create an interface (`AttackStrategy`) and concrete implementations (`SwordAttack`, `BowAttack`).
3. **Integrate it**: The `Character` class will hold a pointer to an `AttackStrategy`.
---
```c++
#include <iostream>
#include <memory>
#include <string>

// 1. The interface for the part that varies
class AttackStrategy {
public:
    virtual void attack(const std::string& target) = 0;
    virtual ~AttackStrategy() = default;
};
```
```c++
// 2. Concrete implementations of the varying behavior
class SwordAttack : public AttackStrategy {
public:
    void attack(const std::string& target) override {
        std::cout << "Swinging a sword at " << target << "!" << std::endl;
    }
};

class BowAttack : public AttackStrategy {
public:
    void attack(const std::string& target) override {
        std::cout << "Firing an arrow at " << target << "!" << std::endl;
    }
};
```
---

```
// 3. The class that stays constant uses the strategy
class Character {
private:
    std::string name;
    std::unique_ptr<AttackStrategy> attack_behavior;

public:
    Character(std::string n, AttackStrategy* as) {
        name = n;
        attack_behavior = as;
    }

    // We can even change the behavior at runtime!
    void setAttackStrategy(std::unique_ptr<AttackStrategy> as) {
        attack_behavior = std::move(as);
    }

    void performAttack(const std::string& target) {
        std::cout << name << " attacks. ";
        attack_behavior->attack(target);
    }
};
```
---
```
// --- Main Usage ---
int main() {
    // A knight with a sword
    Character knight("Sir Arthur", std::make_unique<SwordAttack>());
    knight.performAttack("the dragon");

    // An archer with a bow
    Character archer("Robin", std::make_unique<BowAttack>());
    archer.performAttack("the sheriff");

    // The knight picks up a bow
    knight.setAttackStrategy(std::make_unique<BowAttack>());
    knight.performAttack("the fleeing goblin");

    return 0;
}

```
---
##### Program Against Abstractions, Not Concretions

Write code that depends on **abstract interfaces or base classes**, not on specific implementations.

**Why:**
- Promotes flexibility and loose coupling.
- Makes it easier to change or replace components.
---
###### Example
```c++
class FileLogger {
public:
    void log(const std::string& msg) { /* write to file */ }
};

class App {
    FileLogger logger;  // tightly coupled
};

```
```c++
class Logger {
public:
    virtual void log(const std::string& msg) = 0;
};

class FileLogger : public Logger { /* ... */ };
class ConsoleLogger : public Logger { /* ... */ };

class App {
    Logger* logger;
public:
    App(Logger* l) : logger(l) {}
};

```

---
##### The Hollywood Principle: "Don't call us, we'll call you"

This principle inverts the flow of control. It means that **high-level components in a system call low-level components, but low-level components should not call high-level components directly.** 

This is the fundamental idea behind frameworks. You write specific pieces of code, and the **framework (the high-level component) calls your code at the appropriate time.**

---
###### Example
```C++
#include <iostream>
#include <functional>
#include <string>

// Low-level component: A simple Button
// It provides a hook (`onClick`) but doesn't know who will use it.
class Button {
public:
    // The hook where a high-level component can insert its logic.
    std::function<void()> onClick;

    void simulateClick() {
        std::cout << "[Button]: Click event detected." << std::endl;
        // "We'll call you." The Button calls the provided function.
        if (onClick) {
            onClick();
        }
    }
};

```
```c++
// High-level component: The application that uses the button
class Application {
public:
    void run() {
        Button saveButton;

        // The Application tells the button what to do. "Don't call us..."
        // It provides a lambda function as a callback.
        saveButton.onClick = [this]() {
            this->onSaveDocument();
        };

        // The framework/main loop simulates a user clicking the button
        std::cout << "[Application]: User is about to click 'Save'." << std::endl;
        saveButton.simulateClick();
    }

private:
    // This is the specific logic the Application wants to run.
    void onSaveDocument() {
        std::cout << "[Application]: Now executing the save document logic!" << std::endl;
    }
};
```
---

```
// --- Main Usage ---
int main() {
    Application app;
    app.run();
    return 0;
}

```
---
#### Application to snake problem

Refer 
- [snake version 1](../images/snake_uml.png)
- [snake version 2](../images/snake_uml_enc_what_varies.png)
- [snake version 3](../images/snake_uml_enc_what_varies_hollywood_style.png)