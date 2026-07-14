#include <iostream>
#include <vector>
#include <chrono>
#include <memory>

//------------------------------------
// Domain Model
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

//------------------------------------
// Repository Interface
//------------------------------------
struct IInstallmentRepository {
    virtual ~IInstallmentRepository() = default;
    virtual void persist(const Installment& inst) = 0;
};

//------------------------------------
// Implementation
//------------------------------------
class InstallmentGenerator {
    IInstallmentRepository& repository_;
public:
    explicit InstallmentGenerator(IInstallmentRepository& repo)
        : repository_(repo) {}

    void generateInstallments(const ShoppingCart& cart, int numberOfInstallments) {
        using namespace std::chrono;
        auto nextDue = system_clock::now();
        double perInstallment = cart.getValue() / numberOfInstallments;

        for (int i = 0; i < numberOfInstallments; ++i) {
            nextDue += std::chrono::hours(24 * 30);  // Approx 1 month
            Installment inst(nextDue, perInstallment);
            repository_.persist(inst);
        }
    }
};

//------------------------------------
// Fake Repository (for observation)
//------------------------------------
class MockRepository : public IInstallmentRepository {
public:
    std::vector<Installment> persisted;
    void persist(const Installment& inst) override {
        persisted.push_back(inst);
    }
};

//------------------------------------
// “Test” Simulation
//------------------------------------
int main() {

    MockRepository repo;
    InstallmentGenerator generator(repo);

    ShoppingCart cart(100.0);
    generator.generateInstallments(cart, 10);

    //  Hard to assert anything here unless we inspect the mock.
    std::cout << "Installments persisted: " << repo.persisted.size() << "\n";
}
