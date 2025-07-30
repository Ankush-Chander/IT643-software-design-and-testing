---
delivery date:
  - "[[2025-07-28]]"
---
#### Quick recap
- Programming paradigms
	prodecural => modular => Object oriented => Generic programming

---
#### Agenda
- Design principles
	- Abstraction
	- Encapsulation
	- Decomposition
	- Generalization

---
#### Abstraction
- Helps in **modeling real-world concepts** in problem domain
- Simplifies complexity by **focusing on essential details**
- **Ignores irrelevant** information
---
##### How it"s implemented?
- **Attributes**: Do not disappear over time
- **Behaviours**: Actions/responsibilities of the concept
---
#####  UML representation
![](https://github.com/Ankush-Chander/IT643-software-design-and-testing/raw/main/images/abstraction_uml.png)

---
#### Encapsulation
- **Bundle** data + behaviour
- **Expose** limited functionality via interface
- **Restrict** access to internal details
---
##### How it"s implemented
- Bundle (implicitly via usage of classes)
- Restricted access (use of private/public access classifiers)
- Expose (via setters and getters)
---
##### UML representation
![](https://github.com/Ankush-Chander/IT643-software-design-and-testing/blob/main/images/encapsulation_uml.png?raw=true)

---
##### Advantages
- Internal implementation hidden (black box)
- Only interface matters
- Enables modular and maintainable code
- Getters: Retrieve private data
- Setters: Safely update private data
---
#### Decomposition
- Break down complex system into parts
- Also combine parts to build wholes
- Makes systems easier to understand and manage
---
**Whole-Part Design**
- Each part has specific responsibility
- Class per part; main object coordinates parts 
**Nature of Parts:**
- **Fixed**: Oven has 4 burners
- **Dynamic**: Fridge holds varying food items
- **Recursive**: Kitchen → Oven → Burner
---
**Lifetimes**:
- Parts may have dependent or independent lifetimes
- e.g., Gauge failure disables fridge vs. spoiled food doesn’t
---
#### UML representation
- **Association**: Loose connection  
Eg: Person <-> hotel
- **Aggregation**: Whole–part, loosely bound
- - represented by blank diamond
Eg: Airliner "has-a" Crew
- **Composition**: Whole–part, tightly bound 
- represented by solid diamond
Eg: House "has-a" room

---
#### Generalization
- Remove redundancy
- Capture shared traits in a superclass
- Subclasses extend and specialize
- DRY(Dont Repeart yourself) principle
- YAGNI (You are going to need it once)
---
##### UML representation
- Arrow from subclass → superclass
- Superclass on top
- Subclass only declares new attributes/behaviours
---
##### Java vs C++
| Feature              | Java                 | C++                                  |
| -------------------- | -------------------- | ------------------------------------ |
| Multiple inheritance | Only via interfaces  | Full multiple class inheritance      |
| Interface = no state | ✅                    | ❌ (base classes can hold data)       |
| Diamond problem      | Not possible         | Needs virtual inheritance to resolve |
| Syntax simplicity    | Clean (`implements`) | More powerful, but complex           |

---
##### Simple Inheritance(Java)
```java
class Animal {
    void speak() {
        System.out.println("Animal speaks");
    }
}

class Dog extends Animal {
    void bark() {
        System.out.println("Dog barks");
    }
}

public class Main {
    public static void main(String[] args) {
        Dog d = new Dog();
        d.speak();  // Inherited from Animal
        d.bark();   // Dog-specific
    }
}


```
---
#### Multiple inheritance via interface (Java)
```Java
interface Printer {
    void print(String doc);
}

interface Scanner {
    void scan(String doc);
}

class MultiFunctionDevice implements Printer, Scanner {
    public void print(String doc) {
        System.out.println("Printing: " + doc);
    }

    public void scan(String doc) {
        System.out.println("Scanning: " + doc);
    }
}

public class Main {
    public static void main(String[] args) {
        MultiFunctionDevice mfd = new MultiFunctionDevice();
        mfd.print("JavaDoc.pdf");
        mfd.scan("Receipt.jpg");
    }
}

```

---
##### Simple inheritance (C++)
```c++
#include <iostream>
class Animal {
public:
    void speak() {
        std::cout << "Animal speaks\n";
    }
};

class Dog : public Animal {
public:
    void bark() {
        std::cout << "Dog barks\n";
    }
};

int main() {
    Dog d;
    d.speak();  // Inherited from Animal
    d.bark();   // Dog-specific
}

```
---
##### Multiple inheritance (C++)
```c++
#include <iostream>
#include <string>

class Printer {
public:
    void print(const std::string& doc) {
        std::cout << "Printing: " << doc << "\n";
    }
};

class Scanner {
public:
    void scan(const std::string& doc) {
        std::cout << "Scanning: " << doc << "\n";
    }
};

class MultiFunctionDevice : public Printer, public Scanner {
    // Inherits both print() and scan()
};

int main() {
    MultiFunctionDevice mfd;
    mfd.print("C++Doc.pdf");
    mfd.scan("Invoice.jpg");
}

```