//
// Created by Martin Cooper on 8/24/20.
//

#ifndef STOCKEXCHANGE_ENGINE_H
#define STOCKEXCHANGE_ENGINE_H

#include <queue>
#include "Book.h"
#include "types.h"

struct OrderEvent {
    // allow emplace to construct
    OrderEvent(oid_t id, qty_t shares, EngineType::OrderEventType type, double avgPrice) :
        orderId(id), numShares(shares), type(type), averagePrice(avgPrice) {}

    oid_t orderId;
    qty_t numShares;
    EngineType::OrderEventType type;
    double averagePrice;
};


class Engine {
public:
    Engine(int securityLowPrice, int priceRange, std::queue<OrderEvent> &outputEventQueue) :
        buyBook(securityLowPrice, priceRange),
        sellBook(securityLowPrice, priceRange),
        engineEventQueue(outputEventQueue),
        orderIdMap{} {
        orderIdMap.reserve(priceRange);
    }

    void handleTransaction(oid_t orderId, int limitPrice, qty_t newOrderQuantity, EngineType::OrderType oType);

    void modifyOrderQuantity(oid_t orderId, qty_t newOrderQuantity);

    void cancelOrder(oid_t orderId);

    template<EngineType::OrderType side>
    auto getWorkingBook();

private:
    Book<BookType::OrderSide::BUY> buyBook;
    Book<BookType::OrderSide::SELL> sellBook;
    std::vector<Order *> orderIdMap;
    std::queue<OrderEvent> &engineEventQueue;

    template<EngineType::OrderType side>
    void processLimitOrder(oid_t orderId, int limitPrice, qty_t quantity);


};


#endif //STOCKEXCHANGE_ENGINE_H
