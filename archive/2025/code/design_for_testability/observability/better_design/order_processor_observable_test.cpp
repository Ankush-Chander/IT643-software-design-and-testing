#include "order_processor_observable.h"
#include <gtest/gtest.h>
#include <gmock/gmock.h>

class DummyDelivery : public DeliveryService {
public:
    void scheduleDelivery() override {}     // no need to mock/spy
};

TEST(OrderProcessorTest, ObservablePropertyAllowsDirectAssertion) {
    auto dummyService = std::make_shared<DummyDelivery>();
    OrderProcessor processor(dummyService);

    processor.processOrder();

    EXPECT_TRUE(processor.isReadyForDelivery());   // direct observable assertion
}
