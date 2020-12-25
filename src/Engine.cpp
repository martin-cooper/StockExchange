#include "Engine.h"

void Engine::handleTransaction(
    oid_t orderId,
    int limitPrice,
    qty_t newOrderQuantity,
    EngineType::OrderType oType
) {
    if (oType == EngineType::OrderType::BUY) {
        processLimitOrder<EngineType::OrderType::BUY>(orderId, limitPrice, newOrderQuantity);
    } else if (oType == EngineType::OrderType::SELL) {
        processLimitOrder<EngineType::OrderType::SELL>(orderId, limitPrice, newOrderQuantity);
    }
}

template<EngineType::OrderType side>
auto Engine::getWorkingBook() {
    constexpr auto isBuyBook = side == EngineType::OrderType::BUY;
    using BuyBookType = Book<BookType::OrderSide::BUY>;
    using SellBookType = Book<BookType::OrderSide::SELL>;
    if constexpr (isBuyBook) {
        return std::tuple<SellBookType &, BuyBookType &>(sellBook, buyBook);
    } else {
        return std::tuple<BuyBookType &, SellBookType &>(buyBook, sellBook);
    }
}

template<EngineType::OrderType side>
void Engine::processLimitOrder(oid_t orderId, int limitPrice, qty_t quantity) {
    constexpr bool isBuyOrder = side == EngineType::OrderType::BUY;
    constexpr auto bookType = side == EngineType::OrderType::BUY ? BookType::OrderSide::BUY : BookType::OrderSide::SELL;
    auto books = getWorkingBook<side>();
    auto &book = std::get<0>(books);
    auto &incomingOrderBook = std::get<1>(books);
    auto incomingOrder = new Order(orderId, quantity, limitPrice, bookType);

    orderIdMap.push_back(incomingOrder);

    auto bookIterator = book.iterator();

    // initial order event, Order might not be added to the book
    engineEventQueue.emplace(
        orderId,
        quantity,
        EngineType::OrderEventType::ADDED_TO_BOOK,
        limitPrice
    );

    while (bookIterator.hasNext() && incomingOrder->getUnfilledShares() > 0) {
        Limit *currentLimit = bookIterator.getNext();
        if (isBuyOrder && currentLimit->getPrice() > limitPrice) {
            break;
        } else if (!isBuyOrder && currentLimit->getPrice() < limitPrice) {
            break;
        }
        while (currentLimit->hasOrders() && incomingOrder->getUnfilledShares() > 0) {
            const auto fillResult = currentLimit->fillSharesForHead(incomingOrder->getUnfilledShares());
            const auto sharesFilled = std::get<0>(fillResult);
            const auto topOrderFilled = std::get<1>(fillResult);
            // price improvement for new order
            const int currentLimitPrice = currentLimit->getPrice();

            incomingOrder->fillSharesAtPrice(sharesFilled, currentLimit->getPrice());
            const auto incomingOrderFilled = incomingOrder->getUnfilledShares() == 0;
            const auto &topOrder = currentLimit->peekHead();
            if (topOrderFilled) {
                engineEventQueue.emplace(
                    topOrder.idNumber,
                    sharesFilled,
                    EngineType::OrderEventType::FILL,
                    topOrder.getAverageFillPrice()
                );
            } else {
                engineEventQueue.emplace(
                    topOrder.idNumber,
                    sharesFilled,
                    EngineType::OrderEventType::PARTIAL_FILL,
                    currentLimitPrice
                );
            }
            if (incomingOrderFilled) {
                engineEventQueue.emplace(
                    orderId,
                    sharesFilled,
                    EngineType::OrderEventType::FILL,
                    incomingOrder->getAverageFillPrice()
                );
            } else {
                engineEventQueue.emplace(
                    orderId,
                    sharesFilled,
                    EngineType::OrderEventType::PARTIAL_FILL,
                    currentLimitPrice
                );
            }
            if (topOrderFilled) {
                currentLimit->popHead();
                book.findNewBestPrice();
            }

        }
    }
    if (incomingOrder->getUnfilledShares() == 0) {
        engineEventQueue.emplace(
            orderId,
            quantity,
            EngineType::OrderEventType::FILL,
            incomingOrder->getAverageFillPrice()
        );
    } else {
        incomingOrderBook.addOrderToBook(incomingOrder);
    }
}

void Engine::modifyOrderQuantity(oid_t orderId, qty_t newOrderQuantity) {
    auto order = orderIdMap[orderId];
    if (order->sharesFilled > order->qty - newOrderQuantity) {
        engineEventQueue.emplace(orderId, 0, EngineType::OrderEventType::ORDER_REDUCE_FAIL, 0);
    } else {
        order->qty = newOrderQuantity;
        engineEventQueue.emplace(orderId, newOrderQuantity, EngineType::OrderEventType::ORDER_REDUCE_SUCCESS, 0);
    }
}

void Engine::cancelOrder(oid_t orderId) {
    auto &order = orderIdMap[orderId];
    const bool isBuyBook = order->type == BookType::OrderSide::BUY;
    if (order->type == BookType::OrderSide::BUY) {
        buyBook.removeOrder(*order);
    } else {
        sellBook.removeOrder(*order);
    }
    engineEventQueue.emplace(orderId, 0, EngineType::OrderEventType::CANCEL_SUCCESS, 0);

    if (isBuyBook) {
        buyBook.findNewBestPrice();
    } else {
        sellBook.findNewBestPrice();
    }
}