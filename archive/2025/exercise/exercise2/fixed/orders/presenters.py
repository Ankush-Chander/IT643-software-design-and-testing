import json
from .models import Order # Note the relative import

def to_json(order: Order) -> str:
    """JSON representation logic is also here."""
    return json.dumps({
        "price": order.price,
        "tax": order.tax,
        "shipping_cost": order.shipping_cost
    })
