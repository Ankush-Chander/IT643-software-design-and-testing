#pragma once
#include <string>
#include <memory>
#include "warehouse.hpp"
#include "mail_service.hpp"

/** \brief Represents a customer's order */
class Order {
    int quantity;
    std::string item;
    bool filled;
    std::shared_ptr<MailService> mailService;

public:
    Order(int qty, std::string itm)
        : quantity(qty), item(std::move(itm)), filled(false) {}

    void setMailService(std::shared_ptr<MailService> ms) {
        mailService = std::move(ms);
    }

    bool fill(Warehouse &warehouse) {
        if (warehouse.hasInventory(quantity, item)) {
            warehouse.remove(quantity, item);
            filled = true;
            if (mailService)
                mailService->send("Order filled for " + item);
            return true;
        } else {
            if (mailService)
                mailService->send("Order cannot be filled for " + item);
            filled = false;
            return false;
        }
    }

    bool isFilled() const { return filled; }
};

