#include "gtest/gtest.h"
#include "../lib/order.hpp"
#include "../lib/warehouse.hpp"
#include "../lib/mail_service.hpp"

#include <memory>
#include <string>
#include <unordered_map>

/** 
 * SPY for the Warehouse interface 
 * 
 */
class WarehouseImpl : public Warehouse {
public:
    void add(std::string item, int quantity) {
        inventory[item] += quantity;
    }

    bool hasInventory(int quantity, std::string item) override {
        return inventory[item] >= quantity;
    }

    void remove(int quantity, std::string item) override {
        inventory[item] -= quantity;
    }

    int getInventory(std::string item) override{
        auto it = inventory.find(item);
        return (it != inventory.end()) ? it->second : 0;
    }

private:
    std::unordered_map<std::string, int> inventory;
};


/** SPY for the mail service */
class ConsoleMailService : public MailService {
public:
    void send(std::string message) override {
        sentMessages.push_back(message);
    }

    std::vector<std::string> sentMessages;
};


/** ----------- Tests with real objects (no mocks) ----------- */

TEST(OrderStateTest, OrderIsFilledIfEnoughInWarehouse) {
    const std::string TALISKER = "Talisker";
    const std::string HIGHLAND_PARK = "Highland Park";

    WarehouseImpl warehouse;
    warehouse.add(TALISKER, 50);
    warehouse.add(HIGHLAND_PARK, 25);

    auto mailService = std::make_shared<ConsoleMailService>();

    Order order(50, TALISKER);
    order.setMailService(mailService);
    
    //ACT
    order.fill(warehouse);

    // ASSERT
    ASSERT_TRUE(order.isFilled());
    EXPECT_EQ(0, warehouse.getInventory(TALISKER));

    // Check that the mail service was called
    ASSERT_EQ(1, mailService->sentMessages.size());
    EXPECT_EQ("Order filled for Talisker", mailService->sentMessages[0]);
}

TEST(OrderStateTest, OrderDoesNotRemoveIfNotEnoughInventory) {
    const std::string TALISKER = "Talisker";
    const std::string HIGHLAND_PARK = "Highland Park";

    WarehouseImpl warehouse;
    warehouse.add(TALISKER, 50);
    warehouse.add(HIGHLAND_PARK, 25);

    auto mailService = std::make_shared<ConsoleMailService>();

    Order order(51, TALISKER);
    order.setMailService(mailService);

    order.fill(warehouse);

    ASSERT_FALSE(order.isFilled());
    EXPECT_EQ(50, warehouse.getInventory(TALISKER));

    // Check that the mail service was called
    ASSERT_EQ(1, mailService->sentMessages.size());
    EXPECT_EQ("Order cannot be filled for Talisker", mailService->sentMessages[0]);
}


int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
