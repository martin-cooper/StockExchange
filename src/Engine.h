//
// Created by Martin Cooper on 8/24/20.
//

#ifndef STOCKEXCHANGE_ENGINE_H
#define STOCKEXCHANGE_ENGINE_H

#include <ctime>
#include <memory>
#include "Book.h"
#include "types.h"

struct OrderEvent {
    oid_t orderId;
    std::time_t timestamp;
    qty_t numShares;
    EngineType::OrderEventType type;
    double averagePrice;
};


class Engine {
    using events_t = std::unique_ptr<std::vector<OrderEvent>>;
private:
    Book buyBook;
    Book sellBook;
    events_t handleBuy(oid_t orderId, int limitPrice, qty_t newOrderQuantity);
    events_t handleSell(oid_t orderId, int limitPrice, qty_t newOrderQuantity);

public:
    explicit Engine(std::size_t allocSize) :
        buyBook{Book{allocSize, OrderType::BUY, allocSize}},
        sellBook{Book{allocSize, OrderType::SELL, allocSize}}
    {}
    events_t processIncomingOrder(oid_t orderId, int limitPrice, qty_t quantity, EngineType::OrderType oType);
    Book& getBuyBook() {return buyBook;}
    Book& getSellBook() {return sellBook;}
    friend std::ostream& operator<< (std::ostream &out, const Engine &engine);

};


#endif //STOCKEXCHANGE_ENGINE_H
