#include "order_processor.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using ::testing::AtLeast;
using ::testing::StrictMock;

class MockDeliveryService : public DeliveryService {
public:
    MOCK_METHOD(void, scheduleDelivery, (), (override));
};

TEST(OrderProcessorTest, CallsDeliverySchedule_NotObservableSoUseSpy) {
    auto mockService = std::make_shared<StrictMock<MockDeliveryService>>();

    EXPECT_CALL(*mockService, scheduleDelivery())
        .Times(1);

    OrderProcessor processor(mockService);
    processor.processOrder();
}
