from .models import Order # Note the relative import

def is_valid(order: Order) -> bool:
    """Logic for all order fields is in one place."""
    return order.tax >= 0 and order.shipping_cost >= 0
