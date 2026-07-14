Of course\! Here are the same exercises, converted to Python, to illustrate the code smells and refactoring goals for each component cohesion principle.

-----

### Exercise 1: The Common Reuse Principle (CRP) Violation

**The Smell: The Bloated Utility Module**

A single Python module (`mega_utils.py`) contains a wide variety of unrelated functions. A consumer who needs one simple function is forced to install all the dependencies for the entire module (e.g., `requests`, `psycopg2`), even if they are not used.

#### 👎 Smelly Code (Violation)

Imagine you have a single, massive utility module.

**File: `mega_utils.py` (The God Module)**

```python
# In a shared library/component: 'company_common'
# This component's requirements.txt would need:
# requests
# psycopg2-binary

import base64
import requests  # Heavy dependency for network requests
import psycopg2  # Heavy dependency for database connections

# --- String Utilities ---
def capitalize(input_str: str) -> str:
    if not input_str:
        return input_str
    return input_str.capitalize()

def encode_base64(data: str) -> str:
    return base64.b64encode(data.encode('utf-8')).decode('utf-8')

# --- Network Utilities ---
def http_get(url: str) -> str:
    """Makes an HTTP GET request."""
    print(f"Making an HTTP GET request to {url}")
    response = requests.get(url)
    response.raise_for_status()
    return response.text

# --- Database Utilities ---
def get_db_connection(dsn: str):
    """Gets a PostgreSQL database connection."""
    print("Connecting to database...")
    # ... complex logic to get a DB connection ...
    # return psycopg2.connect(dsn) # dummy
    return None
```

**File: `report_generator.py` (The Consumer)**

```python
# In a separate project/component: 'reporting_service'
# To use just one function, we must install requests and psycopg2!

# We are forced to import from the entire kitchen sink
from company_common import mega_utils

class ReportGenerator:
    def generate_report_title(self, title: str):
        # All I needed was a simple capitalize function...
        formatted_title = mega_utils.capitalize(title)

        print(f"Report Title: {formatted_title}")
        # ...but now my 'reporting_service' has transitive dependencies
        # on 'requests' and 'psycopg2'. A security patch in 'requests'
        # forces me to redeploy my service, even though I don't use it.
```

**Task:** Refactor this code to adhere to the Common Reuse Principle. Break the `mega_utils` module into smaller, more cohesive modules.

-----

#### 👍 Fixed Code (Adherence)

We break the `company_common` library into smaller, installable sub-components or namespaces.

**Component 1: `company_string_utils`**
**File: `string_utils.py`**

```python
# This component has ZERO external dependencies.
import base64

def capitalize(input_str: str) -> str:
    if not input_str:
        return input_str
    return input_str.capitalize()

def encode_base64(data: str) -> str:
    return base64.b64encode(data.encode('utf-8')).decode('utf-8')
```

**Component 2: `company_network_utils`**
**File: `network_utils.py`**

```python
# This component only requires 'requests'.
import requests

def http_get(url: str) -> str:
    """Makes an HTTP GET request."""
    response = requests.get(url)
    response.raise_for_status()
    return response.text
```

*(...and so on for `company_db_utils`)*

**File: `report_generator.py` (The Happy Consumer)**

```python
# In component: 'reporting_service'
# My requirements.txt is now empty, as I only need the built-in string utils.

# Now I only import from the specific component I actually use.
from company_string_utils import string_utils

class ReportGenerator:
    def generate_report_title(self, title: str):
        # The dependency is now lightweight and focused.
        formatted_title = string_utils.capitalize(title)
        print(f"Report Title: {formatted_title}")

        # My component is no longer coupled to network or database libraries.
        # Changes in those components won't affect me.
```

-----

### Exercise 2: The Common Closure Principle (CCP) Violation

**The Smell: Shotgun Surgery**

A single logical change requires you to make small modifications in many different components (in Python, this often means different directories or packages). The classes and functions that change together are not located together.

#### 👎 Smelly Code (Violation)

Imagine a system for processing orders, with logic scattered across a standard but poorly cohesive structure.

**File: `models/order.py` (Data Models component)**

```python
from dataclasses import dataclass

@dataclass
class Order:
    price: float
    tax: float
```

**File: `logic/order_validator.py` (Business Logic component)**

```python
from models.order import Order

def is_valid(order: Order) -> bool:
    """Rule: Tax must be positive."""
    return order.tax >= 0
```

**File: `presenters/order_presenter.py` (UI/API component)**

```python
import json
from models.order import Order

def to_json(order: Order) -> str:
    """Converts the Order object to a JSON string."""
    return json.dumps({
        "price": order.price,
        "tax": order.tax
    })
```

**Change Request:** "We need to add a `shipping_cost` to every order. It must be validated (must be non-negative) and must be included in the JSON output."

To implement this, you must modify files in three different directories: `models/`, `logic/`, and `presenters/`. This is a classic CCP violation.

**Task:** Refactor this structure so that changes related to the `Order` entity are localized to a single component.

-----

#### 👍 Fixed Code (Adherence)

Group all the code that changes for reasons related to the `Order` entity into a single, cohesive `orders` Python package.

**New Package Structure:**

```
/orders/
|-- __init__.py
|-- models.py
|-- validators.py
|-- presenters.py
```

**File: `orders/models.py`**

```python
from dataclasses import dataclass

@dataclass
class Order:
    price: float
    tax: float
    shipping_cost: float  # New field added here
```

**File: `orders/validators.py`**

```python
from .models import Order # Note the relative import

def is_valid(order: Order) -> bool:
    """Logic for all order fields is in one place."""
    return order.tax >= 0 and order.shipping_cost >= 0
```

**File: `orders/presenters.py`**

```python
import json
from .models import Order # Note the relative import

def to_json(order: Order) -> str:
    """JSON representation logic is also here."""
    return json.dumps({
        "price": order.price,
        "tax": order.tax,
        "shipping_cost": order.shipping_cost
    })
```

Now, the entire change is implemented by modifying files within **a single `orders` package**. This package can be tested and released as a single, cohesive unit, fulfilling the Common Closure Principle.

-----

### Exercise 3: The Release Reuse Equivalency Principle (REP) Violation

**The Smell: Reusing Source Code via Relative Paths**

Instead of depending on a versioned, installed package, projects use `sys.path` hacks or relative file paths to access code from a shared directory. This is extremely fragile.

#### 👎 Smelly Structure (Violation)

A file system layout that encourages direct, unversioned source code access.

```
/workspace/
|
+-- /shared-utils/
|   |-- __init__.py
|   |-- string_helpers.py
|
+-- /project-alpha/
|   |-- main.py
|
+-- /project-beta/
    |-- process.py
```

**File: `/project-alpha/main.py`**

```python
import sys
import os

# FRAGILE: This depends on the exact directory structure.
# If someone moves 'project-alpha' or 'shared-utils', this breaks.
sys.path.append(os.path.abspath('../shared-utils'))

from string_helpers import capitalize

print(capitalize("hello from project alpha"))
```

The problem is identical to the previous example: a change in `shared-utils` can unknowingly break `project-alpha` and `project-beta`. There is no formal release or versioning.

**Task:** Propose a new structure and workflow that adheres to the REP using standard Python packaging.

-----

#### 👍 Fixed Structure (Adherence)

The `shared-utils` library is turned into a proper, installable package.

**Step 1: Package `shared-utils`**

The `shared-utils` directory is given a `pyproject.toml` file (the modern standard) or a `setup.py`.

**/shared-utils/pyproject.toml**

```toml
[project]
name = "my-corp-shared-utils"
version = "1.0.0" # The crucial version number!
authors = [
  { name="Your Name", email="you@example.com" },
]
description = "A shared utility library"
```

This package is then built (`python -m build`) and published to a private package repository (like Gemfury, Artifactory, or a private index on PyPI).

**Step 2: Declare Dependencies**

Projects `alpha` and `beta` now declare their dependency on a *specific version* of the package in a `requirements.txt` file. They install it using `pip`.

**/project-alpha/requirements.txt**

```
my-corp-shared-utils==1.0.0
```

**/project-alpha/main.py** (Now clean and robust)

```python
# No more sys.path hacks! The package is installed in the environment.
from string_helpers import capitalize

print(capitalize("hello from project alpha"))
```

**/project-beta/requirements.txt**

```
my-corp-shared-utils==1.0.0
```

**The Workflow:**

1.  A developer fixes a bug in `shared-utils`, updates the version in `pyproject.toml` to `1.0.1`, and publishes the new package.
2.  The teams for `project-alpha` and `project-beta` are notified.
3.  They can now make a conscious decision to update their `requirements.txt` to `my-corp-shared-utils==1.0.1` and run `pip install -r requirements.txt`. They can test this new version in a controlled way before deploying.

This workflow ensures **stability** and **predictability**. Reuse is now tied directly to a formal release process, the cornerstone of REP.
