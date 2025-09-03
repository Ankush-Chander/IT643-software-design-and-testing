def statement(invoice, plays):
    total_amount = 0
    volume_credits = 0
    result = f"Statement for {invoice['customer']}\n"

    def format_amount(amount):
        return "${:.2f}".format(amount)

    for perf in invoice["performances"]:
        play = plays[perf["playID"]]
        this_amount = 0

        match play["type"]:
            case "tragedy":
                this_amount = 40000
                if perf["audience"] > 30:
                    this_amount += 1000 * (perf["audience"] - 30)
            case "comedy":
                this_amount = 30000
                if perf["audience"] > 20:
                    this_amount += 10000 + 500 * (perf["audience"] - 20)
                this_amount += 300 * perf["audience"]
            case _:
                raise ValueError(f"unknown type: {play['type']}")
        # add credits by volume
        volume_credits += max(perf["audience"] - 30, 0)
        # add an extra credit for every 5 comedy spectators
        if play["type"] == "comedy":
            volume_credits += perf["audience"] // 5

        # display the line for this request
        result += f" {play['name']}: {format_amount(this_amount / 100)} ({perf['audience']} seats)\n"
        total_amount += this_amount

    result += f"Amount owed is {format_amount(total_amount / 100)}\n"
    result += f"You earned {volume_credits} credits\n"

    return result
