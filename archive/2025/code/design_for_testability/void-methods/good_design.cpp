#include <iostream>
#include <vector>
#include <chrono>

//------------------------------------
// Same Domain Classes
//------------------------------------
class Installment {
    std::chrono::system_clock::time_point dueDate_;
    double value_;
public:
    Installment(std::chrono::system_clock::time_point dueDate, double value)
        : dueDate_(dueDate), value_(value) {}
    double getValue() const { return value_; }
    std::chrono::system_clock::time_point getDate() const { return dueDate_; }
};

class ShoppingCart {
    double totalValue_;
public:
    explicit ShoppingCart(double totalValue) : totalValue_(totalValue) {}
    double getValue() const { return totalValue_; }
};

struct IInstallmentRepository {
    virtual ~IInstallmentRepository() = default;
    virtual void persist(const Installment& inst) = 0;
};

class InstallmentGenerator {
    IInstallmentRepository& repository_;
public:
    explicit InstallmentGenerator(IInstallmentRepository& repo)
        : repository_(repo) {}

    std::vector<Installment> generateInstallments(const ShoppingCart& cart, int n) {
        using namespace std::chrono;
        std::vector<Installment> generated;
        auto nextDue = system_clock::now();
        double perInstallment = cart.getValue() / n;

        for (int i = 0; i < n; ++i) {
            nextDue += hours(24 * 30);  // ~1 month
            Installment inst(nextDue, perInstallment);
            repository_.persist(inst);
            generated.push_back(inst);
        }
        return generated;
    }
};

//------------------------------------
// Fake Repository
//------------------------------------
class DummyRepository : public IInstallmentRepository {
public:
    void persist(const Installment& inst) override {
        // simulate persistence
    }
};

//------------------------------------
// “Test” Simulation
//------------------------------------
int main() {
    DummyRepository repo;
    InstallmentGenerator generator(repo);
    ShoppingCart cart(100.0);

    auto installments = generator.generateInstallments(cart, 10);

    // Direct assertions — no need for mocks
    std::cout << "Installments generated: " << installments.size() << "\n";
    for (const auto& inst : installments) {
        std::cout << "Value: " << inst.getValue() << "\n";
    }
}
