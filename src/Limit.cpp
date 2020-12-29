#include "Limit.h"

void Limit::popHead() {
    orders.pop_front();
}

Order &Limit::peekHead() {
    return *orders[0];
}

void Limit::addOrder(std::unique_ptr<Order> order) {
    orders.push_back(std::move(order));
    limitVolume += order->qty;
    orderQty++;
}

// returns shares filled for top order
std::tuple<qty_t, bool> Limit::fillSharesForHead(qty_t numShares) {
    auto &topOrder = orders[0];
    const auto topOrderUnfilled = topOrder->getUnfilledShares();
    qty_t sharesFilled{};
    bool topOrderFilled = false;

    if (numShares >= topOrderUnfilled) {
        topOrder->fillSharesAtPrice(topOrderUnfilled, limitPrice);
        sharesFilled = topOrderUnfilled;
        topOrderFilled = true;
    } else {
        topOrder->fillSharesAtPrice(numShares, limitPrice);
        sharesFilled = numShares;
    }

    limitVolume -= sharesFilled;
    return {sharesFilled, topOrderFilled};
}

// lazy deletion
void Limit::clearOrder(Order &order) {
    limitVolume -= order.getUnfilledShares();
    orderQty--;
}