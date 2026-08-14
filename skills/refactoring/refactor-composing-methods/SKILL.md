---
name: refactor-composing-methods
description: Use when methods are too long, contain duplicate code, or have complex expressions that need simplification — covers Extract Method, Inline Method, Extract Variable, and 6 more techniques
---

# Refactor: Composing Methods

## Overview

These 9 techniques break down complex methods into smaller, readable pieces and eliminate duplication. Most frequently used refactoring techniques and the first defense against Bloater smells.

## When to Use

- Method exceeds ~20 lines
- Comments explaining sections (each should be its own method)
- Same code block in multiple places
- Complex expressions hard to understand at a glance
- Temporary variables accumulate and obscure logic

## Quick Reference

| Technique | Problem | Solution |
|-----------|---------|----------|
| Extract Method | Code fragment that can be grouped | Move into a named method |
| Inline Method | Method body is as clear as its name | Replace calls with body |
| Extract Variable | Complex expression hard to understand | Assign to descriptive variable |
| Inline Temp | Temp assigned once from simple expression | Replace references with expression |
| Replace Temp with Query | Temp holds reusable expression result | Extract to method, replace temp with call |
| Split Temporary Variable | Temp assigned more than once (not loop var) | Separate variable per assignment |
| Remove Assignments to Parameters | Method assigns to its parameters | Use a local variable instead |
| Replace Method with Method Object | Tangled locals prevent extraction | Turn method into its own class |
| Substitute Algorithm | Algorithm replaceable with clearer one | Swap the body |

## Techniques in Detail

### 1. Extract Method

The most important refactoring. When in doubt, extract.

**Before:**
```typescript
function printOwing(invoice: Invoice): void {
  let outstanding = 0;

  // print banner
  console.log("***********************");
  console.log("*** Customer Owes ***");
  console.log("***********************");

  // calculate outstanding
  for (const order of invoice.orders) {
    outstanding += order.amount;
  }

  // print details
  console.log(`name: ${invoice.customer}`);
  console.log(`amount: ${outstanding}`);
}
```

**After:**
```typescript
function printOwing(invoice: Invoice): void {
  printBanner();
  const outstanding = calculateOutstanding(invoice);
  printDetails(invoice, outstanding);
}

function printBanner(): void {
  console.log("***********************");
  console.log("*** Customer Owes ***");
  console.log("***********************");
}

function calculateOutstanding(invoice: Invoice): number {
  return invoice.orders.reduce((sum, order) => sum + order.amount, 0);
}

function printDetails(invoice: Invoice, outstanding: number): void {
  console.log(`name: ${invoice.customer}`);
  console.log(`amount: ${outstanding}`);
}
```

**Smells fixed:** Long Method, Duplicate Code, Comments (excessive)

### 2. Inline Method

Reverse of Extract Method -- use when the body is already perfectly clear.

**Before:**
```typescript
function getRating(driver: Driver): number {
  return moreThanFiveLateDeliveries(driver) ? 2 : 1;
}

function moreThanFiveLateDeliveries(driver: Driver): boolean {
  return driver.numberOfLateDeliveries > 5;
}
```

**After:**
```typescript
function getRating(driver: Driver): number {
  return driver.numberOfLateDeliveries > 5 ? 2 : 1;
}
```

### 3. Extract Variable

**Before:**
```typescript
function price(order: Order): number {
  return order.quantity * order.itemPrice -
    Math.max(0, order.quantity - 500) * order.itemPrice * 0.05 +
    Math.min(order.quantity * order.itemPrice * 0.1, 100);
}
```

**After:**
```typescript
function price(order: Order): number {
  const basePrice = order.quantity * order.itemPrice;
  const quantityDiscount = Math.max(0, order.quantity - 500) * order.itemPrice * 0.05;
  const shipping = Math.min(basePrice * 0.1, 100);
  return basePrice - quantityDiscount + shipping;
}
```

### 4. Inline Temp / Replace Temp with Query

Inline Temp removes a trivial temp; Replace Temp with Query promotes a meaningful expression to a reusable method.

**Replace Temp with Query:**
```typescript
// Before
const basePrice = order.quantity * order.itemPrice;
if (basePrice > 1000) return basePrice * 0.95;
return basePrice * 0.98;

// After
if (basePrice(order) > 1000) return basePrice(order) * 0.95;
return basePrice(order) * 0.98;

function basePrice(order: Order): number {
  return order.quantity * order.itemPrice;
}
```

**Rule:** Trivial, used once -> Inline Temp. Meaningful and reusable -> Replace Temp with Query.

### 5. Split Temporary Variable

**Before:**
```typescript
let temp = 2 * (height + width);
console.log(temp);
temp = height * width;
console.log(temp);
```

**After:**
```typescript
const perimeter = 2 * (height + width);
console.log(perimeter);
const area = height * width;
console.log(area);
```

### 6. Remove Assignments to Parameters

Critical for immutability.

**Before:**
```typescript
function discount(inputVal: number, quantity: number): number {
  if (quantity > 50) inputVal -= 2;  // mutating parameter!
  return inputVal;
}
```

**After:**
```typescript
function discount(inputVal: number, quantity: number): number {
  const result = quantity > 50 ? inputVal - 2 : inputVal;
  return result;
}
```

### 7. Replace Method with Method Object

Use when Extract Method is impossible due to tangled local variables.

**Before:**
```typescript
class Order {
  price(): number {
    let primaryBasePrice: number;
    let secondaryBasePrice: number;
    let tertiaryBasePrice: number;
    // ... long computation using all three variables intertwined ...
  }
}
```

**After:**
```typescript
class PriceCalculator {
  constructor(
    private readonly order: Order,
    private primaryBasePrice: number = 0,
    private secondaryBasePrice: number = 0,
    private tertiaryBasePrice: number = 0
  ) {}

  compute(): number {
    this.calculatePrimary();
    this.calculateSecondary();
    this.calculateTertiary();
    return this.primaryBasePrice + this.secondaryBasePrice + this.tertiaryBasePrice;
  }

  private calculatePrimary(): void { /* ... */ }
  private calculateSecondary(): void { /* ... */ }
  private calculateTertiary(): void { /* ... */ }
}
```

### 8. Substitute Algorithm

**Before:**
```typescript
function foundPerson(people: string[]): string {
  for (let i = 0; i < people.length; i++) {
    if (people[i] === "Don") return "Don";
    if (people[i] === "John") return "John";
    if (people[i] === "Kent") return "Kent";
  }
  return "";
}
```

**After:**
```typescript
function foundPerson(people: string[]): string {
  const candidates = new Set(["Don", "John", "Kent"]);
  return people.find(p => candidates.has(p)) ?? "";
}
```

## Decision Flowchart

```dot
digraph composing {
  rankdir=TB;
  start [label="Method too complex?" shape=diamond];
  q1 [label="Can you identify\na code fragment\nto name?" shape=diamond];
  q2 [label="Is a temp variable\nobscuring things?" shape=diamond];
  q3 [label="Are local variables\ntoo tangled to\nextract methods?" shape=diamond];
  q4 [label="Is a trivial method\njust adding\nindirection?" shape=diamond];

  em [label="Extract Method" shape=box];
  ev [label="Extract Variable" shape=box];
  rtq [label="Replace Temp\nwith Query" shape=box];
  rmmo [label="Replace Method\nwith Method Object" shape=box];
  im [label="Inline Method" shape=box];
  sa [label="Substitute Algorithm" shape=box];

  start -> q1 [label="yes"];
  start -> sa [label="whole algorithm\ncan be replaced"];
  q1 -> em [label="yes"];
  q1 -> q2 [label="no"];
  q2 -> ev [label="expression complex\nbut single use"];
  q2 -> rtq [label="temp reused\nor method-worthy"];
  q2 -> q3 [label="no"];
  q3 -> rmmo [label="yes"];
  q3 -> q4 [label="no"];
  q4 -> im [label="yes"];
}
```

## Common Mistakes

| Mistake | Fix |
|---------|-----|
| Extracting methods that are too small (single-line getters) | Only extract when the name adds clarity beyond the code itself |
| Naming extracted methods by implementation (`calcStep1`) | Name by intent (`calculateDiscount`) |
| Extracting but creating Long Parameter Lists | If 4+ params needed, consider Extract Class or Parameter Object |
| Inlining methods that provide useful abstraction | Only inline when the body is as clear as the name |
| Forgetting to run tests after each extraction | Every refactoring step should be followed by a test run |
