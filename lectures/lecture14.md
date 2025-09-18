---
delivery date:
  - "[[2025-09-10]]"
---
## Agenda
- Refactoring techniques(continued)

---
#### Move function
**Problem**

- Good design = modularity → changes require understanding _only a small part_.
- If a function depends more on another module’s data/functions → it’s in the wrong place.
- Current context may no longer fit → hurts encapsulation & clarity.

**Solution**
- **Move function** to the context where it “belongs”:
    - Where its most-used data lives.
    - Where its main callers are.
    - Into a new module/class if it defines a new responsibility.
---
#### Replace Conditional with Polymorphism
```javascript
switch (bird.type) {
	case 'EuropeanSwallow':
		return "average";
	case 'AfricanSwallow':
		return (bird.numberOfCoconuts > 2) ? "tired" : "average";
	case 'NorwegianBlueParrot':
		return (bird.voltage > 100) ? "scorched" : "beautiful";
	default:
		return "unknown";
```
to
```javascript
class EuropeanSwallow {
	get plumage() {
		return "average";
}
class AfricanSwallow {
	get plumage() {
		return (this.numberOfCoconuts > 2) ? "tired" : "average";
}
class NorwegianBlueParrot {
	get plumage() {
		return (this.voltage > 100) ? "scorched" : "beautiful";
}
```
---
**Problem**
- Complex `if/else` or `switch` → hard to read, duplicate across functions.
- Behavior really depends on _type_ (e.g., Book vs Music vs Food).
**Solution**
- Replace conditionals with **class hierarchy + polymorphism**:
    - Base class: common/default logic.
    - Subclasses: override behavior for each variant.
    - Factory: creates the right subclass instance.
---
#### Replace Type Code with Subclasses

```javascript
function createEmployee(name, type) {
return new Employee(name, type);
}
```
to
```javascript
function createEmployee(name, type) {
	switch (type) {
		case "engineer": return new Engineer(name);
		case "salesman": return new Salesman(name);
		case "manager": return new Manager (name);
}
```

---

- Symptom: You classify objects with a **type code** (`enum`, `string`, `int`).
- Example:
    - `Employee(type=ENGINEER)`
    - `Employee(type=MANAGER)`
- Issues:
    - Type-specific conditionals everywhere (`if (type==ENGINEER) …`).
    - Some fields/methods valid only for certain types.
- **Solution**
	- Replace type code with **subclasses**:
	    - `Engineer : Employee`
	    - `Manager : Employee`
	    - `Salesman : Employee`