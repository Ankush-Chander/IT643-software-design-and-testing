from dataclasses import dataclass

@dataclass
class Order:
    price: float
    tax: float
    shipping_cost: float  # New field added here
