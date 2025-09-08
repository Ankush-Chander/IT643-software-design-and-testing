---
delivery date:
  - "[[2025-09-08]]"
---
#### Quick recap
1. Deep vs shallow modules
2. Refactoring demo
---
#### Agenda
1. Refactoring
2. Code smells
3. Common refactoring techniques
---
#### Refactoring
> Refactoring (noun): a change made to the **internal structure of software** to make it easier to understand and cheaper to  modify **without changing its observable behavior**.

> Refactoring (verb): to restructure software by applying a series of refactorings without changing its observable behavior.
---
#### Why refactor?
- improves the design of software
- makes software easier to understand
- helps find bugs
- helps program faster
---
#### Code smells
> “If it stinks, change it.”  
> — Grandma Beck, discussing child-rearing philosophy

- It"s easier to explain how to do refactoring than when.
- No hard and fast rules
- Comes with practice and intuition

---
##### Long function
- **Small functions live longer** → easier to read, maintain, and reuse.
- **Modern languages remove call overhead** → no excuse for long functions.
- **Good naming reduces need to read body**.
- **Rule of thumb**: When you feel like adding a comment → Extract a Function.
- **Not about length** → about semantic distance between intent and implementation(what vs. how).
Long functions = Bad smell → aggressively decompose into meaningful, named functions.

---
##### Duplicate code
- Duplication = more reading, harder changes, higher bug risk.
- Every copy must be checked → wasted effort.
**Guiding principle:** Short, meaningful, unified functions = healthier codebase.
---
##### Divergent Change

- **Goal of structure**: Make change easy → one clear place to modify.
- **Smell**: When one module changes for _different reasons_ (e.g., database vs. financial logic).
- **Problem**: Mixed contexts → every change touches unrelated code → harder to understand & maintain.
- **Better design**: Separate contexts into distinct modules/classes.
---
##### Shotgun Surgery

- **Smell**: Opposite of _Divergent Change_.
- Every change → many small edits across multiple classes/modules.
- Hard to track, easy to miss updates.
---
##### Comments
- **Comments are good** → but often used as _deodorant_ to hide bad code.
- Heavy comments usually signal underlying _bad smells_.
- **First step**: Refactor → often eliminates the need for comments.
**When comments help**
- To explain _why_ something is done (not what).
- To flag uncertainty or future concerns.
**Principle:** _Write clean code that explains itself — use comments for intent, not explanation._
---
#### Common refactoring techniques
---
##### Extract function

![](../images/refactoring/extract_function.png)
```javascript
function printOwing(invoice) {
	printBanner();
	let outstanding = calculateOutstanding();
	//print details
	console.log(`name: ${invoice.customer}`);
	console.log(`amount: ${outstanding}`);
}
```
to
```javascript
function printOwing(invoice) {
	function printDetails(outstanding) {
		console.log(`name: ${invoice.customer}`);
		console.log(`amount: ${outstanding}`);
}
	printBanner();
	let outstanding = calculateOutstanding();
	printDetails(outstanding);

}
```

---
##### Inline function 
```javascript
function moreThanFiveLateDeliveries(driver) {
	return driver.numberOfLateDeliveries > 5;
}

function getRating(driver) {
	return moreThanFiveLateDeliveries(driver) ? 2 : 1;
}

```
to

```javascript
function getRating(driver) {
	return (driver.numberOfLateDeliveries > 5) ? 2 : 1;
}
```
---
- Inverse of `Extract Function`

- **When to Use**
	- Function’s body = just as clear as its name.
	- Too much delegation → hard to trace logic.
	- Want to regroup/refactor functions.

- **Mechanics**
	1. Ensure it’s **not polymorphic** (no subclass overrides).
	2. Find all callers.
	3. Replace call with body → test after each.
	4. Remove function definition.
	5. Inline gradually if tricky (multiple returns, recursion).
- **Principle:** _Indirection is good only when it adds clarity — inline when it doesn’t._

---
##### Slide statements
```javascript
const pricingPlan = retrievePricingPlan();
const order = retreiveOrder();
let charge;
const chargePerUnit = pricingPlan.unit;
```
to

```javascript
const pricingPlan = retrievePricingPlan();
const chargePerUnit = pricingPlan.unit;
const order = retreiveOrder();
let charge;
```
---
- **Problem**
	- Related code is **scattered**, mixed with unrelated logic.
	- Harder to understand, modify, or extract into functions.
- **Solution**
	- **Move related statements together** so intent is clearer.
	- Often a preparatory step for **Extract Function**.
 **Principle:** _Group related logic together → clarity first, then refactor further._

---
##### Replace temp with query

```javascript
const basePrice = this._quantity * this._itemPrice;
return basePrice > 1000 ? basePrice * 0.95 : basePrice * 0.98;

```
to
```javascript
getBasePrice() {
	return this._quantity * this._itemPrice; 
}

// call function instead of temp variable
return this.getBasePrice() > 1000 ? this.getBasePrice() * 0.95 : this.basePrice * 0.98;

```
---

**Motivation**
- Easier function extraction (no temps to pass around).
- Stronger boundaries → fewer dependencies & side effects.
- Eliminates duplicate calculation logic.
- Best inside a class (shared context for queries).
---
##### Split Loop
```javascript
let averageAge = 0;
let totalSalary = 0;
for (const p of people) {
	averageAge += p.age;
	totalSalary += p.salary;
}
averageAge = averageAge / people.length;
```
to
```javascript
let totalSalary = 0;
for (const p of people) {
	totalSalary += p.salary;
}

let averageAge = 0;
for (const p of people) {
	averageAge += p.age;
}
averageAge = averageAge / people.length;
```

---
**Problem**
- One loop doing **multiple things at once**.
- Makes modifications harder → must understand _all_ behaviors.
- Leads to cluttered code with multiple outputs/side effects.
**Solution**
- **Split loop into separate loops**, each doing one task.
- Improves clarity and maintainability.   
- Often followed by **Extract Function** on each loop.
---
##### Split phase 
```javascript
const orderData = orderString.split(/\s+/);
const productPrice = priceList[orderData[0].split("-")[1]];
const orderPrice = parseInt(orderData[1]) * productPrice;
```
to
```javascript
function parseOrder(aString) {
	const values = aString.split(/\s+/);
	return ({productID: values[0].split("-")[1], quantity: parseInt(values[1]),
});
}
function price(order, priceList) {
	return order.quantity * priceList[order.productID];
}

const orderRecord = parseOrder(order);
const orderPrice = price(orderRecord, priceList);


```
---
**Clues**
- Different parts use different data/functions.
- Sequential steps doing _significantly different_ work.
**Mechanics**
1. Extract second phase into its own function.
2. Add intermediate data structure (passed between phases).
3. Move relevant parameters/fields into the structure.
4. Extract first phase separately.

**Principle:** _Separate concerns into phases → easier to reason about, test, and extend._

---
#### References

1. Chapter 1, [Refactoring, Second edition](https://martinfowler.com/books/refactoring.html) by Martin Fowler and Kent Beck
2. [Refactoring example in Javascript](https://github.com/vitorfranca/Refactoring-Martin_Fowler/tree/main/Chapter-1) 