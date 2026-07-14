class PerformanceCalculator:
    def __init__(self, a_performance, play):
        self.performance = a_performance
        self.play = play

    def get_amount(self):
        raise "Subclass should implement"

    def get_volume_credits(self):
        raise "Subclass should implement"

class TragedyCalculator(PerformanceCalculator):
    def get_amount(self):
        result = 40000
        if self.performance["audience"] > 30:
            result += 1000 * (self.performance["audience"] - 30)
        return result

    def get_volume_credits(self):
        result = 0
        result += max(self.performance["audience"] - 30, 0)
        return result

class ComedyCalculator(PerformanceCalculator):
    def get_amount(self):
        result = 30000
        if self.performance["audience"] > 20:
            result += 10000 + 500 * (self.performance["audience"] - 20)
        result += 300 * self.performance["audience"]
        return result

    def get_volume_credits(self):
        result = 0
        result += max(self.performance["audience"] - 30, 0)
        # add an extra credit for every 5 comedy spectators
        if self.performance['play']["type"] == "comedy":
            result += self.performance["audience"] // 5
        return result


class LoveStoryCalculator(PerformanceCalculator):
    pass


def get_statement_data(invoice, plays):
    def play_for(perf):
        return plays[perf["playID"]]

    def get_statement_calculator(perf):
        # print(perf[])
        if perf["play"]["type"] == "comedy":
            return ComedyCalculator(perf, play_for(perf))
        elif perf["play"]["type"] == "tragedy":
            return TragedyCalculator(perf, play_for(perf))
        
        else:
            raise "Play type not supported"

    def enrich_performance(perf):
        perf["play"] = play_for(perf)
        calculator = get_statement_calculator(perf) # gePerformanceCalculator(perf, play_for(perf))
        perf["amount"] = calculator.get_amount()
        perf["volume_credits"] = calculator.get_volume_credits()
        return perf

    def get_total_amount(statement_data):
        total_amount = 0
        for perf in statement_data["performances"]:
            total_amount += perf['amount']
        return total_amount

    def get_total_volume_credits(statement_data):
        result = 0
        for perf in statement_data["performances"]:
            # this_amount = amount_for(perf)
            result += perf['volume_credits']
        return result


    statement_data = {}
    statement_data['customer'] = invoice['customer']
    statement_data["performances"] = [enrich_performance(perf) for perf in invoice["performances"]]
    statement_data["total_amount"] = get_total_amount(statement_data)
    statement_data["total_volume_credits"] = get_total_volume_credits(statement_data)
    return statement_data

