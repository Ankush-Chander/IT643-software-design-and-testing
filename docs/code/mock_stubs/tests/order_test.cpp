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
    MOCK_METHOD(bool, hasInventory, (int, std::string), (override)); // query => stub
    MOCK_METHOD(void, remove, (int, std::string), (override)); // command => mock
    MOCK_METHOD(int, getInventory, (std::string), (override)); // query => stub
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
    order.fill(warehouse);

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
    order.fill(warehouse);

    // Assert
    ASSERT_FALSE(order.isFilled());
}

int main(int argc, char **argv) {
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

/*
 * - Pros:
 * Almost no boilerplate. Expressive matchers/cardinalities — EXPECT_CALL(warehouse, remove(_,_)).Times(0) cleanly proves "must not remove".
 * Auto-fails on unexpected/uncalled expectations. Compiler enforces the mock matches the interface.
 * - Cons: Tightly couples test to how fill() calls its collaborators — refactoring breaks green tests.
 *  Over-specification risk; tests can pass while the system is broken.
 * Framework learning curve, and requires the code be designed against interfaces (virtual, DI via setMailService).
 */
