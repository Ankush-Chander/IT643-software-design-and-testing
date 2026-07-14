import json
from src.statement import statement



def test_play_performance():
    with open("expected.txt", "r") as f:
        expected = f.read()
    with open("plays.json") as f:
        plays = json.load(f)
    with open("invoices.json") as f:
        invoices = json.load(f)
    result = statement(invoices[0], plays)
    assert result == expected

