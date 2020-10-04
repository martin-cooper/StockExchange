//
// Created by Martin Cooper on 8/16/20.
//

#ifndef STOCKEXCHANGE_LIMIT_H
#define STOCKEXCHANGE_LIMIT_H

#include <vector>
#include <string>
#include "types.h"

struct Order {
    Order(oid_t idNumber, qty_t qty, int limitPrice):
        idNumber {idNumber}, qty {qty}, limitPrice {limitPrice}, sharesFilled {0}, cumulativeFillPrice {0},
        orderTime {std::time(nullptr)}, prev {nullptr}, next {nullptr}
    {}
    const oid_t idNumber;
    qty_t qty;
    const int limitPrice;
    qty_t sharesFilled;
    // Sum of numFilled * price, prevents loss of data due to floating point arithmetic
    int cumulativeFillPrice;
    const std::time_t orderTime;
    Order *prev;
    Order *next;
    qty_t getUnfilledShares() const {return qty - sharesFilled;}
};

class Limit {
private:
    int limitPrice;
    //number of outstanding orders
    unsigned int orderQty;
    //cumulative quantity of shares in outstanding orders
    qty_t limitVolume;
    Order *head;
    Order *tail;

public:
    Limit() :
        limitPrice {0}, limitVolume {0}, orderQty {0}, head {nullptr}, tail {nullptr}
    {}
    ~Limit() {
        Order* runner = head;
        while (runner != nullptr) {
            auto temp = runner;
            runner = runner->next;
            delete temp;
        }
        head = nullptr;
        tail = nullptr;
    }
    int getPrice() const {return limitPrice;}
    qty_t getVolume() const {return limitVolume;}
    unsigned int getOrderQty() const {return orderQty;}
    void setPrice(int price) {limitPrice = price;}
    Order *getHead() {return head;}

    void addOrder(Order *order);
    void removeOrder(Order *order);
    qty_t reduceOrderQty(Order *order, qty_t sharesToReduceBy);
    qty_t partialFillOrder(Order *order, qty_t sharesFilled);
    friend std::ostream& operator<< (std::ostream &out, const Limit &level);
};


#endif //STOCKEXCHANGE_LIMIT_H
