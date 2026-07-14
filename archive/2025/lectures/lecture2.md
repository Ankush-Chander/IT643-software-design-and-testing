---
delivery date:
  - "[[2025-07-23]]"
---
### Quick recap
- Course overview
- Project expectation
- Software life-cycle
- Object Oriented Thinking
---
### Agenda
- Design in Software Process
- Conceptual design vs Technical design 
- CRC
---


### Design in the Software Process

| Stage                 | Artifacts           |
| --------------------- | ------------------- |
| Requirement gathering | User interviews     |
| Conceptual Design     | Conceptual mockups  |
| Technical design      | Technical diagrams  |
| Implementation        | Documentation/ Code |

---
### Understanding Client Needs

- **Requirements** are conditions or capabilities that **must be implemented** in a product, based on client or user requests. They are the **starting point** of a project.
- Requires **actively probing**
- Identifying potential **trade-offs** the client might need to make (e.g., sacrificing a feature for faster program execution).

---
### Conceptual design

- **Conceptual designs** are based on initial requirements.
- Recognize appropriate **components, connections, and responsibilities** of the software product.
- visual notations providing **initial thoughts** on how requirements will be met.  Can be hand-drawn or created using computer tools.
- Example: For a house, components are the lot, house, and rooms. Connections are how rooms are accessible. Responsibilities include providing power/water (house) or space for items (kitchen). 
---

### Technical Design

- While conceptual design outlines major components, connections, and responsibilities, technical design describes **how these responsibilities are met**.
- **split components into smaller and smaller components**, until a level is reached where detailed design is possible.
- The final result is that **each component will have its technical details specified**.
- Example: For a kitchen (a component of a house), technical design might specify flooring material that is easy to clean, given a client's cooking habits.

---
### Trade offs

- Beyond functional requirements, **quality attributes** define how well functionality must work.
- Design decisions involve **trade-offs** between quality attributes like performance, convenience, and security, which need to be balanced.
- Example: A secure front door with many locks might be inconvenient to use; good design balances security with convenience and performance.
---
### Defining System Behavior and Performance

- **Functional requirements** describe **what the system or application is expected to do**.
    - A key quality for functional requirements is **correctness**.
- **Non-functional requirements** specify **how well the system or application does what it does**.
    - These can include performance, resource usage, and efficiency, measurable from the running software.
---

### Class Responsibility Collaborator (CRC) Cards

- CRC cards are designed with **three sections**:
    - **Class Name:** At the top of the card. This is where the name of the candidate component goes.
    - **Responsibilities:** On the left side of the card. These are the tasks the component needs to perform.
    - **Collaborators:** On the right side of the card. These indicate other classes that the class at the top interacts with to fulfill its responsibilities.
---

### CRC Card: `Order`

| Class   | Responsibilities                                                | Collaborators                    |
| ------- | --------------------------------------------------------------- | -------------------------------- |
| `Order` | - Maintain list of items<br>- Calculate total<br>- Submit order | `Customer`<br>`PaymentProcessor` |
