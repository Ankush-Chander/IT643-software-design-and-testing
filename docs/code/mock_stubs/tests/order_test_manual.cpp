#include "gtest/gtest.h"
#include "../lib/mail_service.hpp"
#include "../lib/warehouse.hpp"
#include "../lib/order.hpp"

#include <memory>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

/** Manual mock for the Warehouse interface */
class ManualMockWarehouse : public Warehouse {
public:
    struct Call {
        std::string method;
        int quantity;
        std::string item;
    };

    std::vector<Call> calls;
    bool hasInventoryResult = false; // configurable behavior

    bool hasInventory(int quantity, std::string item) override {
        calls.push_back({"hasInventory", quantity, item});
        return hasInventoryResult;
    }

    void remove(int quantity, std::string item) override {
        calls.push_back({"remove", quantity, item});
    }

    // Helper methods for checking calls
    bool wasCalled(const std::string &method) const {
        return std::any_of(calls.begin(), calls.end(),
            [&](const Call &c){ return c.method == method; });
    }

    int callCount(const std::string &method) const {
        return std::count_if(calls.begin(), calls.end(),
            [&](const Call &c){ return c.method == method; });
    }

    int getInventory(std::string item) override {
        calls.push_back({"getInventory", 0, item});
        return 0;
    }
};

/** Manual mock for the MailService interface */
class ManualMockMailService : public MailService {
public:
    std::vector<std::string> sentMessages;

    void send(std::string message) override {
        sentMessages.push_back(message);
    }

    bool wasCalled() const { return !sentMessages.empty(); }
    std::string lastMessage() const {
        return sentMessages.empty() ? "" : sentMessages.back();
    }
};


// Test: Successful order
TEST(OrderTest, Fill_SuccessfulOrder_ManualMock) {
    // Arrange
    ManualMockWarehouse warehouse;
    auto mailService = std::make_shared<ManualMockMailService>();

    Order order(50, "Talisker");
    order.setMailService(mailService);

    // Configure behavior of mock
    warehouse.hasInventoryResult = true;

    // Act
    order.fill(warehouse);

    // Assert
    ASSERT_TRUE(order.isFilled());

    // Verify interactions
    EXPECT_EQ(warehouse.callCount("hasInventory"), 1);
    EXPECT_EQ(warehouse.callCount("remove"), 1);
    EXPECT_TRUE(mailService->wasCalled());
}

// Test: Insufficient inventory
TEST(OrderTest, Fill_InsufficientInventory_ManualMock) {
    // Arrange
    ManualMockWarehouse warehouse;
    auto mailService = std::make_shared<ManualMockMailService>();

    Order order(10, "Lagavulin");
    order.setMailService(mailService);

    // Configure behavior
    warehouse.hasInventoryResult = false;

    // Act
    order.fill(warehouse);

    // ASSERT
    ASSERT_FALSE(order.isFilled());

    // Verify interactions
    EXPECT_EQ(warehouse.callCount("hasInventory"), 1);
    EXPECT_EQ(warehouse.callCount("remove"), 0);
    EXPECT_TRUE(mailService->wasCalled());
    EXPECT_NE(mailService->lastMessage().find("cannot be filled"), std::string::npos);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
 * - Pros:
 * Zero framework dependency — works anywhere, fully transparent (students see exactly what a mock is). Total control over return values.
 - Cons:
 Boilerplate scales badly — every interface needs its own recorder + helpers.
 You'll reinvent argument matching, call ordering, strictness. Easy to write a mock that silently drifts from the real interface.
 Purely post-hoc verification, no "unexpected call" detection.
 */
