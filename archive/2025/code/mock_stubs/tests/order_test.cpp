#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "../lib/mail_service.hpp"
#include "../lib/warehouse.hpp"
#include "../lib/order.hpp"

using ::testing::Return;
using ::testing::_; // Matcher for parameters

/** Mock for the Warehouse interface */
class MockWarehouse : public Warehouse {
public:
    MOCK_METHOD(bool, hasInventory, (int, std::string), (override));
    MOCK_METHOD(void, remove, (int, std::string), (override));
    MOCK_METHOD(int, getInventory, (std::string), (override));
};

/** Mock for the MailService interface */
class MockMailService : public MailService {
public:
    MOCK_METHOD(void, send, (std::string), (override));
};

TEST(OrderTest, Fill_SuccessfulOrder) {
    // setup data
    MockWarehouse warehouse;
    auto mailService = std::make_shared<MockMailService>();

    Order order(50, "Talisker");
    order.setMailService(mailService);

    // setup expectations
    EXPECT_CALL(warehouse, hasInventory(50, "Talisker"))
        .Times(1)
        .WillOnce(Return(true));

    EXPECT_CALL(warehouse, remove(50, "Talisker"))
        .Times(1);

    EXPECT_CALL(*mailService, send(_))  // Message content not asserted
        .Times(1);

    // ACT
    bool result = order.fill(warehouse);
    
    ASSERT_TRUE(result);
    ASSERT_TRUE(order.isFilled());
}

TEST(OrderTest, Fill_InsufficientInventory) {
    // Arrange data
    MockWarehouse warehouse;
    auto mailService = std::make_shared<MockMailService>();

    Order order(10, "Lagavulin");
    order.setMailService(mailService);

    // Arrange expectations
    EXPECT_CALL(warehouse, hasInventory(10, "Lagavulin"))
        .Times(1)
        .WillOnce(Return(false));

    EXPECT_CALL(warehouse, remove(_, _)).Times(0);

    EXPECT_CALL(*mailService, send(::testing::HasSubstr("cannot be filled")))
        .Times(1);

    // Act
    bool result = order.fill(warehouse);

    // Assert
    ASSERT_FALSE(result);
    ASSERT_FALSE(order.isFilled());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

