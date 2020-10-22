//
// Created by Martin Cooper on 8/24/20.
//
#include <iostream>
#include "Engine.h"


Engine::events_t
Engine::processIncomingOrder(oid_t orderId, int limitPrice, qty_t quantity, EngineType::OrderType oType) {
    if (oType == EngineType::BUY || oType == EngineType::SELL) {
        return handleTransaction(orderId, limitPrice, quantity, oType);
    }
}

Engine::events_t Engine::handleTransaction(
        oid_t orderId,
        int limitPrice,
        qty_t newOrderQuantity,
        EngineType::OrderType oType) {

    auto result = std::make_unique<std::vector<OrderEvent>>();
    auto incomingOrder = new Order(orderId, newOrderQuantity, limitPrice);
    auto &bookToAddNewOrder {oType == EngineType::BUY ? buyBook : sellBook};
    auto &newOrderLimit {bookToAddNewOrder.addOrder(incomingOrder)};
    auto &operatingBook {oType == EngineType::BUY ? sellBook : buyBook};
    auto const ask = operatingBook.getBestPriceAmount();
    auto const ind = operatingBook.getBestPriceIndex();
    auto currentTime = std::time(nullptr);
    auto &bookData = operatingBook.getBookData();

    if (ask > limitPrice || ind == -1) {
        result->push_back(OrderEvent{
                .orderId=orderId,
                .timestamp=currentTime,
                .numShares=newOrderQuantity,
                .type=EngineType::ADDED_TO_BOOK,
                .averagePrice=static_cast<double>(limitPrice),
        });
        return std::move(result);
    }

    auto bookEnd {oType == EngineType::BUY ? bookData.end() : bookData.begin()};

    // for a buy order we want to iterate forward through the book data
    // for a sell order we want to iterate backwards
    auto shouldLoopEnd {[oType, bookEnd, limitPrice](auto &it) {
        if (oType == EngineType::BUY) {
            return it < bookEnd && it->getPrice() <= limitPrice;
        } else {
            return it > bookEnd && it->getPrice() >= limitPrice;
        }
    }};

    auto advanceLoop{[oType](auto &it) {
        if (oType == EngineType::BUY) {
            it++;
        } else {
            it--;
        }
    }};

    for (auto it{bookData.begin() + ind}; shouldLoopEnd(it); advanceLoop(it)) {
        if (it->getOrderQty() == 0) {
            continue;
        }
        auto runner = it->getHead();
        while (runner != nullptr) {
            auto unfilled {incomingOrder->getUnfilledShares()};
            // new order completely filled, current iterating order partially filled
            // can safely return
            if (unfilled < runner->getUnfilledShares()) {
                bookToAddNewOrder.fillOrder(incomingOrder);
                operatingBook.fillSharesForOrder(incomingOrder, unfilled);
                result->push_back(OrderEvent{
                        .orderId=orderId,
                        .timestamp=currentTime,
                        .numShares=incomingOrder->sharesFilled,
                        .type=EngineType::FILL,
                        .averagePrice=1.0 * incomingOrder->cumulativeFillPrice / incomingOrder->sharesFilled,
                });
                result->push_back(OrderEvent{
                        .orderId=runner->idNumber,
                        .timestamp=currentTime,
                        .numShares=unfilled,
                        .type=EngineType::PARTIAL_FILL,
                        .averagePrice=1.0 * runner->cumulativeFillPrice / unfilled,
                });
                newOrderLimit.removeOrder(incomingOrder);
                return std::move(result);
                // current and new orders perfectly filled
            } else if (unfilled == runner->getUnfilledShares()) {
                bookToAddNewOrder.fillOrder(incomingOrder);
                runner = runner->next;
                operatingBook.fillOrder(runner);
                result->push_back(OrderEvent{
                        .orderId=orderId,
                        .timestamp=currentTime,
                        .numShares=incomingOrder->sharesFilled,
                        .type=EngineType::FILL,
                        .averagePrice=1.0 * incomingOrder->cumulativeFillPrice / incomingOrder->sharesFilled,
                });
                result->push_back(OrderEvent{
                        .orderId=runner->idNumber,
                        .timestamp=currentTime,
                        .numShares=unfilled,
                        .type=EngineType::FILL,
                        .averagePrice=1.0 * runner->cumulativeFillPrice / runner->qty,
                });
                newOrderLimit.removeOrder(incomingOrder);
                it->removeOrder(runner);
                return std::move(result);
            } else {
                result->push_back(OrderEvent{
                        .orderId=runner->idNumber,
                        .timestamp=currentTime,
                        .numShares=unfilled,
                        .type=EngineType::FILL,
                        .averagePrice=1.0 * runner->cumulativeFillPrice / runner->qty,
                });
                runner = runner->next;
            }
        }

    }
    result->push_back(OrderEvent{
            .orderId=orderId,
            .timestamp=std::time(nullptr),
            .numShares=incomingOrder->getUnfilledShares(),
            .type=EngineType::ADDED_TO_BOOK,
            .averagePrice=1.0 * incomingOrder->cumulativeFillPrice / incomingOrder->qty,
    });
    return std::move(result);
}

std::ostream &operator<<(std::ostream &out, const Engine &engine) {
    out << "Buy book:" << std::endl << engine.buyBook << std::endl
              << "---------------------------------------------------------------" << std::endl;
    out << "Sell book:" << std::endl << engine.sellBook << std::endl
              << "---------------------------------------------------------------" << std::endl;
    return out;
}

std::ostream& operator << (std::ostream &out, const OrderEvent &event) {
    const auto eventType = event.type;
    const std::string type = [eventType](){
        switch(eventType) {
            case EngineType::ADDED_TO_BOOK:
                return "added to book";
            case EngineType::PARTIAL_FILL:
                return "partial fill";
            case EngineType::FILL:
                return "fill";
        }
    }();
    out << "Order with id: " << event.orderId << " to " << type << " executed at a quantity of " << event.numShares <<
    " and an average price " << event.averagePrice << std::endl;
    return out;
}
