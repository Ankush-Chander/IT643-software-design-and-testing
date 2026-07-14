#pragma once
#include <memory>

class DeliveryService {
public:
    virtual ~DeliveryService() = default;
    virtual void scheduleDelivery() = 0;     // we want to check that this happens
};

class OrderProcessor {
public:
    OrderProcessor(std::shared_ptr<DeliveryService> delivery)
        : delivery_(delivery) {}

    void processOrder() {
        // ... some logic ...
        delivery_->scheduleDelivery();  // we need to verify this was called
    }

private:
    std::shared_ptr<DeliveryService> delivery_;
};
