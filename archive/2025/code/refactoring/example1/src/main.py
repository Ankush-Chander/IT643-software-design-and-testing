import json
from statement import statement

with open('plays.json') as f:
    plays = json.load(f)

with open('invoices.json') as f:
    invoices = json.load(f)

if __name__ == '__main__':
    print(statement(invoices[0], plays), end='')
