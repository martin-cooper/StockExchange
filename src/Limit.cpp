#include "Limit.h"

void Limit::addOrder(const std::shared_ptr<Order> &order) {
    orders.push_back(order);
    limitVolume += order->getOrderQuantity();
    orderQty++;
}

// returns shares filled for top order
qty_t Limit::fillSharesForHead(qty_t numShares) {
    auto &topOrder = orders[0];
    const auto topOrderUnfilled = topOrder->getUnfilledShares();
    if (numShares >= topOrderUnfilled) {
        topOrder->fillSharesAtPrice(topOrderUnfilled, limitPrice);
        return topOrderUnfilled;
    } else {
        topOrder->fillSharesAtPrice(numShares, limitPrice);
        return numShares;
    }
}