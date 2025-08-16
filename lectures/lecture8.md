#### Quick recap
- Solid principles

---
####  Big picture


   
   ![](https://user-images.githubusercontent.com/6892666/65833569-bb34fc00-e29f-11e9-8516-79cbd9f8f07b.png)
   ---
   


#### Mid term goal
- What is a **component**?
- **Cohesion** and **Coupling** 
- **Component cohesion principles**:
    - Reuse/Release Equivalence Principle (REP)
    - Common Closure Principle (CCP)
    - Common Reuse Principle (CRP)
- Connects to real-world **e-commerce case study**, SOLID, and microservices.
    

---

#### What is a component

- **A component** = modular, self-contained, independently deployable unit of software.
- Encapsulate specific set of functionality
- Physical deployment artifact: `.jar` (Java), `.dll` (.NET), packaged JS library.
- Like Lego blocks:
    - Standard interface.
    - Internal details hidden.
    - Can be assembled with others.

---

#### Components as Products

- A component is not just deployment — it is a **unit of productization**.
- Moves thinking from **code sharing** → **product lifecycle management**.
- Implies responsibilities like:
    - Versioning & release management.
    - Communicating changes to consumers.
    - Providing stability and support.
---

#### Characteristics of a Well-Designed Component
- **Reusability**: Works in multiple contexts without modification.
- **Encapsulation**: Hide internal details; expose only stable interfaces.
- **Independence**: Minimal dependencies enable parallel development & independent deployment.
- **Replaceability**: Can swap with another implementation if interface is honored.   

---
#### Cohesion and Coupling: The Twin Pillars

- Core forces in all design levels.
- **Cohesion** = how well internal elements belong together.
- **Coupling** = dependency between components.
- Goal: **Maximize cohesion** and **minimize coupling**.
---

##### Component Cohesion

- High cohesion = clear, focused purpose.
- Low cohesion = unrelated responsibilities, harder to maintain.
- Cohesion should come from **business capability** alignment, not just technical layers.
- Domain-driven grouping avoids cross-component change ripple.
---

##### Cohesion Spectrum

| Type                              | Description                | Example                                                                                                                                                                                             |
| --------------------------------- | -------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Functional** (Most desirable)   | All parts serve one task   | A `PaymentProcessor` component containing methods like `validateCreditCard`, `authorizePayment`, and `captureFunds`. All elements serve the single function of processing a payment.                |
| **Sequential**                    | Output of one feeds next   | ReportGenerator: fetch → aggregate → format.                                                                                                                                                        |
| **Communicational**               | Operate on same data       | CustomerManager: address update & order history.                                                                                                                                                    |
| **Procedural**                    | Grouped by execution order | An AppInitializer component that first checks for a valid license, then loads user preferences, and finally establishes a database connection. The tasks are related only by their execution order. |
| **Temporal**                      | Executed at same time      | SystemShutdown routines.                                                                                                                                                                            |
| **Logical**                       | Same category, diff. tasks | InputHandler for keyboard/mouse/network.  The functions are all "input," but they are otherwise unrelated.                                                                                          |
| **Coincidental**(Least desirable) | Arbitrary grouping         | CommonUtilities with unrelated methods.                                                                                                                                                             |

---

##### Component Coupling

- **Tight coupling** = changes ripple across components.
- **Loose coupling** = minimal dependencies, easier change.
- Exists on a **spectrum** — some coupling types are more harmful.
    

---

##### Coupling Spectrum

| Type                         | Description                   | Example                                            |
| ---------------------------- | ----------------------------- | -------------------------------------------------- |
| **Data** _(least harmful)_   | Pass primitives only          | TaxCalculator(price, region).                      |
| **Stamp**                    | Pass complex object, use part | Mailer(User) only needs email & name.              |
| **Control**                  | Pass flags to control logic   | sort(data, directionFlag).                         |
| **External**                 | Depends on specific tech      | Oracle-specific SQL usage.                         |
| **Common**                   | Share global state            | Global Configuration object.                       |
| **Content** _(most harmful)_ | Access/modify internals       | Direct variable modification in another component. |

---

##### Cohesion & Coupling Together

- **High cohesion → naturally loose coupling**.
- Cohesive components encapsulate related responsibilities.
- Reduces need for complex interconnections.
- Example: Payment component & Inventory component interact via **simple messages**, not deep dependencies.


---
### Cohesion principles
#### The Reuse/Release Equivalence Principle (REP)
- **Purpose:** Establishes the relationship between **reusing code** and **releasing it**.
- **Core Statement:** _The granule of reuse is the granule of release_.
- Means:
    - The smallest reusable unit **must** be the same as the smallest released and tracked unit.
    - Reuse is **not** just copying code — it’s using a **released, versioned artifact**.
---

#### Why REP Matters
- **Problem with Copy-Paste Reuse:**
    - Copies diverge over time.
    - Bug fixes and improvements in the original are **not** propagated.
- **REP Solution:**
    - Reuse via **formal release process**.
    - Consumers depend on a **versioned artifact**, not loose source files.

---

#### REP Implications
- **Versioning & Tracking:**
    - Use a clear system (e.g., **Semantic Versioning**).
    - Unique version numbers for precise dependency control.
- **Release Communication:**
    - Notify consumers about changes.
    - Provide release notes so teams can decide whether to upgrade.
- **Building Trust:**
    - Consumers gain confidence that the component is **maintained, predictable, and supported**.

---

#### REP in Practice: Package Managers
- **Java (Maven/Gradle):**
    - `implementation 'org.springframework:spring-core:5.3.23'`
    - Depends on a **specific, released JAR** from Maven Central.
- **JavaScript (npm/Yarn):**
    - `"react": "^18.2.0"` in `package.json`
    - Installs a **versioned package** from npm registry.
- **iOS (CocoaPods):**
    - `pod 'Alamofire', '~> 5.6'`
    - Ensures stable, released version of the library.

---
#### The Common Closure Principle (CCP)
- **Definition:**  
    _"Gather into components those classes that change for the same reasons and at the same times.  
    Separate classes that change for different reasons/times."_
    
- CCP = **Single Responsibility Principle (SRP)** at the **component level**.
- Each component should have **one reason to change**.   

---
##### CCP Benefits
- **Improved Maintainability:**
    - Changes for one requirement are contained within a single component.
    - Reduces complexity and cognitive load.
- **Minimized Deployment Impact:**
    - Only the changed component needs recompilation, testing, and deployment.
    - Other components remain unaffected, reducing risk and cost.

---


#### The Common Reuse Principle (CRP)
- **Focus:** Prevent consumers from depending on things they don’t need.
- **Core Statement:**
    - _"The classes in a component are reused together. If you reuse one, you reuse them all."_
    - Or: _"Don’t force users to depend on things they don’t use."_
- Component-level equivalent of **Interface Segregation Principle (ISP)**.
- **Example:** 
	- *Containers* are often reused along with their *iterators* so it makes sense to keep them in one component.
---

#### CRP in Context
- ISP warns against **fat interfaces** → CRP warns against **fat components**.
- Goal: Group only classes that are **always reused together**.
- Benefits:
    - Leaner, more focused components.
    - Reduced risk and complexity for consumers.

---

#### Benefit 1: Avoiding Dependency Bloat

- If a consumer needs one class but the component has many unrelated ones:
    - Consumer takes on **entire component** as a dependency.
    - May inherit **irrelevant external dependencies**.
- Consequences:
    - Larger application size.
    - Longer compile/build times.
    - Higher risk of **version conflicts**.
---
#### Benefit 2: Preventing Unnecessary Release Churn

- Problem: Mixed stable and volatile classes in one component.
- If volatile class changes:
    - Whole component gets a **new release**.
    - All consumers are notified.
    - Even those who don’t use the changed class must re-validate.
- Result: Wasted time, unnecessary effort.

---
#### Tension diagram for Component cohesion

![](https://miro.medium.com/v2/resize:fit:828/format:webp/1*jg5VnxcRGs9_t02mB5UUvA.png)
Image credits: [Clean Architecture](https://medium.com/@anca.rebeca/clean-architecture-component-cohesion-principles-8c9722abd9bd)


#### References
1. [How to Learn Software Design and Architecture | The Full-stack Software Design & Architecture Map](https://khalilstemmler.com/articles/software-design-architecture/full-stack-software-design)
2. [Chapter 13: Clean Architecture](https://www.amazon.in/Clean-Architecture-Craftsmans-Software-Structure/dp/0134494164)