//
// Created by Martin Cooper on 8/24/20.
//

#ifndef STOCKEXCHANGE_MATCHINGENGINE_H
#define STOCKEXCHANGE_MATCHINGENGINE_H

#include <queue>
#include <unordered_map>
#include "Book.h"
#include "types.h"

struct OrderEvent {
    // allow emplace to construct
    OrderEvent(oid_t first, oid_t second, qty_t shares, EngineType::OrderEventType type, double avgPrice) :
        firstOrder(first), secondOrder(second), numShares(shares), type(type), averagePrice(avgPrice) {}

    oid_t firstOrder;
    oid_t secondOrder;
    qty_t numShares;
    EngineType::OrderEventType type;
    double averagePrice;

    friend std::ostream& operator<<(std::ostream &out, OrderEvent &event);
};


class MatchingEngine {
public:
    MatchingEngine(int securityLowPrice, int priceRange, std::queue<OrderEvent> &outputEventQueue) :
        buyBook(securityLowPrice, priceRange),
        sellBook(securityLowPrice, priceRange),
        engineEventQueue(outputEventQueue),
        orderIdMap{} {}

    std::optional<int> getBid() const {
        return buyBook.getBestPriceAmount();
    }

    std::optional<int> getAsk() const {
        return sellBook.getBestPriceAmount();
    }

    void processIncomingEvent(oid_t orderId, int limitPrice, qty_t newOrderQuantity, EngineType::OrderType oType);

    template<EngineType::OrderType side>
    auto getWorkingAndIncomingBook();

private:
    Book<BookType::OrderSide::BUY> buyBook;
    Book<BookType::OrderSide::SELL> sellBook;
    std::unordered_map<oid_t, Order*> orderIdMap;
    std::queue<OrderEvent> &engineEventQueue;

    template<EngineType::OrderType side>
    void processLimitOrder(oid_t orderId, int limitPrice, qty_t quantity);
    void modifyOrderQuantity(oid_t orderId, qty_t newOrderQuantity);
    void cancelOrder(oid_t orderId);


};


#endif //STOCKEXCHANGE_MATCHINGENGINE_H
