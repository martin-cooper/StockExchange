//
// Created by Martin Cooper on 8/16/20.
//

#ifndef STOCKEXCHANGE_LIMIT_H
#define STOCKEXCHANGE_LIMIT_H

#include <deque>
#include <string>
#include "Order.h"
#include "types.h"

class Limit {
private:
    const int limitPrice;
    //number of outstanding orders
    unsigned int orderQty;
    //cumulative quantity of shares in outstanding orders
    qty_t limitVolume;
    std::deque<std::shared_ptr<Order>> orders;


public:
    explicit Limit(int price) :
            limitPrice{price}, limitVolume{0}, orderQty{0}, orders() {}

    int getPrice() const {
        return limitPrice;
    }

    qty_t getVolume() const {
        return limitVolume;
    }

    unsigned int getOrderQty() const {
        return orderQty;
    }

    void popHead() {
        orders.pop_front();
    }

    void addOrder(const std::shared_ptr<Order> &order);

    qty_t fillSharesForHead(qty_t numShares);

    friend std::ostream &operator<<(std::ostream &out, const Limit &level);
};


#endif //STOCKEXCHANGE_LIMIT_H
