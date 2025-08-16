import json
from models.order import Order

def to_json(order: Order) -> str:
    """Converts the Order object to a JSON string."""
    return json.dumps({
        "price": order.price,
        "tax": order.tax
    })
