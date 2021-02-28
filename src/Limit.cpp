#include <utility>

#include "../include/engine/Limit.h"

void Limit::popHead() {
    orders.pop_front();
}

Order &Limit::peekHead() {
    return *orders[0];
}

void Limit::addOrder(std::unique_ptr<Order> order) {
    limitVolume += order->getUnfilledShares();
    orderQty++;
    orders.push_back(std::move(order));
}

// returns shares filled for top order
std::tuple<qty_t, bool> Limit::fillSharesForHead(qty_t numShares) {
    auto &topOrder{orders[0]};
    const auto topOrderUnfilled{topOrder->getUnfilledShares()};
    qty_t sharesFilled{std::min(numShares, topOrderUnfilled)};
    bool topOrderFilled = false;

    topOrder->fillSharesAtPrice(sharesFilled, limitPrice);
    if (sharesFilled == topOrderUnfilled) {
        topOrderFilled = true;
        orderQty--;
    }

    limitVolume -= sharesFilled;
    return {sharesFilled, topOrderFilled};
}

// lazy deletion
void Limit::clearOrder(Order &order) {
    limitVolume -= order.getUnfilledShares();
    order.qty = order.sharesFilled;
    orderQty--;
}