#pragma once
#include <string>

/** \brief Interface representing a Warehouse */
class Warehouse {
public:
    virtual ~Warehouse() = default;

    /// Check if warehouse has given quantity of an item
    virtual bool hasInventory(int quantity, std::string item) = 0;

    /// Remove given quantity of an item from inventory
    virtual void remove(int quantity, std::string item) = 0;

    /// Get inventory quantity of an item
    virtual int getInventory(std::string item) = 0;
};

