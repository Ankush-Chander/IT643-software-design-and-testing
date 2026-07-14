#include <gtest/gtest.h>

// Class under test
class BankAccount {
public:
    BankAccount() : balance(0) {}

    void Deposit(int amount) {
        if (amount > 0) {
            balance += amount;
        }
    }

    bool Withdraw(int amount) {
        if (amount > 0 && amount <= balance) {
            balance -= amount;
            return true;
        }
        return false;  // insufficient funds or invalid amount
    }

    int GetBalance() const {
        return balance;
    }

private:
    int balance;
};


// Fixture for BankAccount
class BankAccountTest : public ::testing::Test {
protected:
    void SetUp() override {
        account = new BankAccount();
    }

    void TearDown() override {
        delete account;
    }

    BankAccount* account;
    // int x = 0;
};

// Tests
TEST_F(BankAccountTest, InitialBalanceIsZero) {
    EXPECT_EQ(account->GetBalance(), 0);
    // EXPECT_EQ(x, 1);
}

TEST_F(BankAccountTest, DepositIncreasesBalance) {
    account->Deposit(100);
    EXPECT_EQ(account->GetBalance(), 100);
}

TEST_F(BankAccountTest, WithdrawReducesBalance) {
    account->Deposit(200);
    bool success = account->Withdraw(50);
    EXPECT_TRUE(success);
    EXPECT_EQ(account->GetBalance(), 150);
}

TEST_F(BankAccountTest, WithdrawFailsForInsufficientFunds) {
    account->Deposit(50);
    bool success = account->Withdraw(100);
    EXPECT_FALSE(success);
    EXPECT_EQ(account->GetBalance(), 50);
}

// g++ -o bank_test bank_test.cpp -lgtest -lgtest_main -pthread
