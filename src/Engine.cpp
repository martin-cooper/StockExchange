//
// Created by Martin Cooper on 8/24/20.
//

#include "Engine.h"


Engine::events_t Engine::processIncomingOrder(oid_t orderId, int limitPrice, qty_t quantity, EngineType::OrderType oType) {
    if (oType == EngineType::BUY){
        return handleBuy(orderId, limitPrice, quantity);
    }
}
/*
Engine::events_t Engine::handleBuy(oid_t orderId, int limitPrice, qty_t newOrderQuantity) {
    auto result = std::vector<OrderEvent>{};
    auto newOrder{new Order{
            orderId,
            newOrderQuantity,
            limitPrice,
    }};
    auto ask = sellBook.getBestPriceAmount();
    auto ind = sellBook.getBestPriceIndex();
    // cannot match order or this is the first order in the book
    buyBook.addOrder(newOrder);
    if (limitPrice < ask || ind == -1) {
        result.push_back(OrderEvent{
           orderId,
           std::time(nullptr),
           0,
           EngineType::ADDED_TO_BOOK,
           0,
        });
    } else {
        qty_t& sharesFilled{newOrder->sharesFilled};
        int& cumFillPrice{newOrder->cumulativeFillPrice};

        int currentIndex{ind};
        auto const book{sellBook.getBookData()};
        while (sharesFilled != newOrderQuantity && currentIndex < book.size()) {
            Limit const currentLimit = book[ind];
            Order* const runner = currentLimit.getHead();
            int currentLimitPrice = currentLimit.getPrice();
            // the orders at this limit are too high in price, we cannot fill
            if (currentLimitPrice > limitPrice) {
                break;
            }
            while(runner != nullptr && sharesFilled != newOrderQuantity) {
                // how many shares left in the current order there are to be filled
                qty_t const currentOrderQty{runner->qty - runner->sharesFilled};
                oid_t const currentLimitOId{runner->idNumber};
                auto sharesUnfilled{newOrderQuantity - sharesFilled};
                //Current order on the limit is completely filled itself, either partially or fully filling the new order
                if (currentOrderQty <= sharesUnfilled) {
                    runner->sharesFilled += currentOrderQty;
                    runner->cumulativeFillPrice += currentOrderQty * currentLimitPrice;
                    cumFillPrice += currentOrderQty * currentLimitPrice;
                    double averageFill{static_cast<double>(runner->cumulativeFillPrice) / runner->sharesFilled};
                    result.push_back(OrderEvent{
                            currentLimitOId,
                            time(nullptr),
                            runner->sharesFilled,
                            EngineType::FILL,
                            averageFill,
                    });
                    sellBook.fillOrder(currentLimitOId);
                    //Order on the limit fills the new order but it itself is not filled, add a partial fill
                } else {
                    runner->sharesFilled += sharesUnfilled;
                    runner->cumulativeFillPrice += sharesUnfilled * currentLimitPrice;
                    cumFillPrice += sharesUnfilled * currentLimitPrice;
                    sellBook.fillSharesForOrder(currentLimitOId, sharesUnfilled);
                    result.push_back(OrderEvent{
                            currentLimitOId,
                            std::time(nullptr),
                            sharesUnfilled,
                            EngineType::PARTIAL_FILL,
                            static_cast<double>(limitPrice),
                    });
                }
                sharesFilled += currentOrderQty;
            }
            ++currentIndex;
        }
        if (sharesFilled == newOrderQuantity) {
            result.push_back(OrderEvent{
                orderId,
                std::time(nullptr),
                newOrderQuantity,
                EngineType::FILL,
                static_cast<double>(cumFillPrice) / newOrderQuantity,
            });
        } else {
            result.push_back(OrderEvent{
                orderId,
                std::time(nullptr),
                sharesFilled,
                EngineType::PARTIAL_FILL,
                static_cast<double>(cumFillPrice) / (sharesFilled),
            });
            newOrder->sharesFilled = sharesFilled;
            newOrder->cumulativeFillPrice = cumFillPrice;
            result.push_back(OrderEvent{
                orderId,
                std::time(nullptr),
                newOrderQuantity - sharesFilled,
                EngineType::ADDED_TO_BOOK,
                static_cast<double>(limitPrice),
            });
        }
    }
    return result;

}
*/

Engine::events_t Engine::handleBuy(oid_t orderId, int limitPrice, qty_t newOrderQuantity) {
    auto result {std::vector<OrderEvent>{}};
    auto incomingOrder {new Order{orderId, newOrderQuantity, limitPrice}};
    buyBook.addOrder(incomingOrder);
    auto ask = sellBook.getBestPriceAmount();
    auto ind = sellBook.getBestPriceIndex();

    if (ask > limitPrice || ind == -1) {
        result.push_back(OrderEvent{
                orderId,
                std::time(nullptr),
                0,
                EngineType::ADDED_TO_BOOK,
                0,
        });
        return result;
    }
}