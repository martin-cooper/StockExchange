//
// Created by Martin Cooper on 8/24/20.
//
#include <iostream>
#include "Engine.h"


Engine::events_t Engine::processIncomingOrder(oid_t orderId, int limitPrice, qty_t quantity, EngineType::OrderType oType) {
    if (oType == EngineType::BUY){
        return handleBuy(orderId, limitPrice, quantity);
    }
}

Engine::events_t Engine::handleBuy(oid_t orderId, int limitPrice, qty_t newOrderQuantity) {
    auto result {std::make_unique<std::vector<OrderEvent>>()};
    auto incomingOrder {new Order{orderId, newOrderQuantity, limitPrice}};
    auto &newOrderLimit {buyBook.addOrder(incomingOrder)};
    auto ask = sellBook.getBestPriceAmount();
    auto ind = sellBook.getBestPriceIndex();
    auto& sellBookData {sellBook.getBookData()};
    auto const bookSize {sellBookData.size()};
    if (ask > limitPrice || ind == -1) {
        result->push_back(OrderEvent{
                .orderId=orderId,
                .timestamp=::time(nullptr),
                .numShares=0,
                .type=EngineType::ADDED_TO_BOOK,
                .averagePrice=0,
        });
        return std::move(result);
    }
    for (int i {ind}; i < bookSize && sellBookData[i].getPrice() <= limitPrice; ++i) {
        Limit &limit {sellBookData[i]};
        if (limit.getOrderQty() == 0) {
            continue;
        }
        auto runner {limit.getHead()};
        while (runner != nullptr) {
            auto unfilled {incomingOrder->getUnfilledShares()};
            newOrderLimit.partialFillOrder(incomingOrder, unfilled);
            limit.partialFillOrder(runner, unfilled);
            auto currentTime {std::time(nullptr)};
            // new order completely filled, current iterating order partially filled
            // can safely return
            if (unfilled < runner->getUnfilledShares()) {

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
                limit.removeOrder(runner);
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

std::ostream& operator<< (std::ostream &out, const Engine &engine) {
    std::cout << "Buy book:" << std::endl << engine.buyBook << std::endl
    << "---------------------------------------------------------------" << std::endl;
    std::cout << "Sell book:" << std::endl << engine.sellBook << std::endl
              << "---------------------------------------------------------------" << std::endl;
    return out;
}