//
// Created by Martin Cooper on 8/16/20.
//

#ifndef STOCKEXCHANGE_LIMIT_H
#define STOCKEXCHANGE_LIMIT_H

#include <deque>
#include <tuple>
#include <memory>

#include "Order.h"
#include "types.h"

class Limit {
private:
    const int limitPrice;
    //number of outstanding orders
    unsigned int orderQty;
    //cumulative quantity of shares in outstanding orders
    qty_t limitVolume;
    std::deque<std::unique_ptr<Order>> orders;

public:
    explicit Limit(int price) :
        limitPrice{price}, limitVolume{0}, orderQty{0}, orders{} {}

    int getPrice() const {
        return limitPrice;
    }

    qty_t getVolume() const {
        return limitVolume;
    }

    unsigned int getOrderQty() const {
        return orderQty;
    }

    bool hasOrders() const {
        return orderQty != 0;
    }

    const auto &getOrders() const {
        return orders;
    }

    void popHead();

    Order &peekHead();

    void addOrder(std::unique_ptr<Order> order);

    void clearOrder(Order &order);

    std::tuple<qty_t, bool> fillSharesForHead(qty_t numShares);

};


#endif //STOCKEXCHANGE_LIMIT_H
