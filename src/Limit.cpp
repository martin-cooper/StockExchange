//
// Created by Martin Cooper on 8/16/20.
//
#include <iostream>
#include "Limit.h"

void Limit::addOrder(Order *order) {
    if (head == nullptr) {
        head = order;
        tail = order;
        order->next = nullptr;
        order->prev = nullptr;
    } else if (head->next == nullptr) {
        order->next = nullptr;
        order->prev = head;
        head->next = order;
        tail = order;
    } else {
        tail->next = order;
        order->prev = tail;
        tail = order;
    }
    ++orderQty;
    limitVolume += order->qty;
}

void Limit::removeOrder(Order *order) {
    if (order == nullptr) {
        return;
    } else if (order == head) {
        if (order == tail) {
            head = nullptr;
            tail = nullptr;
        } else {
            head = head->next;
        }
    } else if (order == tail) {
        tail->prev->next = nullptr;
        tail = order;
    } else {
        order->prev->next = order->next;
    }
    --orderQty;
    auto unfilled {order->getUnfilledShares()};
    if (unfilled != 0) {
        partialFillOrder(order, unfilled);
    }
    delete order;
}

// returns new order quantity
qty_t Limit::reduceOrderQty(Order *order, qty_t sharesToReduceBy) {
    order->qty -= sharesToReduceBy;
    limitVolume -= sharesToReduceBy;
    return order->qty;
}

// returns number of unfilled shares
qty_t Limit::partialFillOrder(Order *order, qty_t sharesFilled) {
    order->sharesFilled += sharesFilled;
    limitVolume -= sharesFilled;
    order->cumulativeFillPrice += sharesFilled * limitPrice;
    return order->getUnfilledShares();
}

std::ostream& operator<< (std::ostream &out, const Limit &level) {
    out << "Price: " << level.limitPrice << ", Number of orders: " << level.orderQty << ", Volume: " << level.limitVolume;
    out << std::endl;
    Order* runner = level.head;
    for (std::size_t i{0}; i < level.orderQty / 5 + 1; ++i) {
        for (std::size_t j{0}; j < 5 && runner != nullptr; ++j) {
            out << "[ID: " << runner->idNumber << ", Vol: " << runner->getUnfilledShares() << "], ";
            runner = runner->next;
        }
        out << std::endl;
    }
    return out;
}
