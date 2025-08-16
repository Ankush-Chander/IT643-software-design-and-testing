from models.order import Order

def is_valid(order: Order) -> bool:
    """Rule: Tax must be positive."""
    return order.tax >= 0
