#pragma once

#include <memory>

class DeliveryService {
public:
    virtual ~DeliveryService() = default;
    virtual void scheduleDelivery() = 0;
};

class OrderProcessor {
public:
    OrderProcessor(std::shared_ptr<DeliveryService> delivery)
        : delivery_(delivery), readyForDelivery_(false) {}

    void processOrder() {
        // ... some logic ...
        readyForDelivery_ = true;          // state becomes observable
        delivery_->scheduleDelivery();
    }

    // ---- added observable property ----
    bool isReadyForDelivery() const {
        return readyForDelivery_;
    }

private:
    std::shared_ptr<DeliveryService> delivery_;
    bool readyForDelivery_;
};
