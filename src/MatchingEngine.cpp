#include <utility>

#include "../include/engine/MatchingEngine.h"

void MatchingEngine::processIncomingEvent(
    oid_t orderId,
    int limitPrice,
    qty_t newOrderQuantity,
    EngineType::OrderType oType
) {
    if (oType == EngineType::OrderType::BUY) {
        processLimitOrder<EngineType::OrderType::BUY>(orderId, limitPrice, newOrderQuantity);
    } else if (oType == EngineType::OrderType::SELL) {
        processLimitOrder<EngineType::OrderType::SELL>(orderId, limitPrice, newOrderQuantity);
    } else if (oType == EngineType::OrderType::CANCEL_ORDER) {
        cancelOrder(orderId);
    } else if (oType == EngineType::OrderType::ORDER_REDUCE) {
        modifyOrderQuantity(orderId, newOrderQuantity);
    }
}

// first element is the book which the incoming order is matching against
// second element is the book which the incoming order is being placed
template<EngineType::OrderType side>
auto MatchingEngine::getWorkingAndIncomingBook() {
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
void MatchingEngine::processLimitOrder(oid_t orderId, int limitPrice, qty_t quantity) {
    constexpr bool isBuyOrder = side == EngineType::OrderType::BUY;
    constexpr auto bookType = side == EngineType::OrderType::BUY ? BookType::OrderSide::BUY : BookType::OrderSide::SELL;
    auto books = getWorkingAndIncomingBook<side>();
    auto &workingBook = std::get<0>(books);
    auto &incomingOrderBook = std::get<1>(books);
    auto incomingOrder = std::make_unique<Order>(orderId, quantity, limitPrice, bookType);
    orderIdMap.emplace(orderId, incomingOrder.get());

    auto bookIterator = workingBook.iterator();

    // initial order event, Order might not be added to the book
    engineEventQueue.emplace(
        orderId,
        0,
        quantity,
        EngineType::OrderEventType::ADDED_TO_BOOK,
        limitPrice
    );
    // iterate through valid limits
    while (bookIterator.hasNext() && incomingOrder->getUnfilledShares() > 0) {
        Limit *currentLimit = bookIterator.getNext();
        if (!currentLimit
            || (isBuyOrder && currentLimit->getPrice() > limitPrice)
            || (!isBuyOrder && currentLimit->getPrice() < limitPrice)
            ) {
            break;
        }
        // iterate through orders on limit
        while (currentLimit->hasOrders() && incomingOrder->getUnfilledShares() > 0) {
            const auto fillResult = currentLimit->fillSharesForHead(incomingOrder->getUnfilledShares());
            const auto sharesFilled = std::get<0>(fillResult);
            // cleans up cancelled orders
            if (sharesFilled == 0) {
                currentLimit->popHead();
                continue;
            }

            const auto topOrderFilled = std::get<1>(fillResult);
            // price improvement for new order
            const int currentLimitPrice = currentLimit->getPrice();

            incomingOrder->fillSharesAtPrice(sharesFilled, currentLimitPrice);
            const auto incomingOrderFilled = incomingOrder->getUnfilledShares() == 0;
            const auto &topOrder = currentLimit->peekHead();

            engineEventQueue.emplace(
                orderId,
                topOrder.idNumber,
                sharesFilled,
                EngineType::OrderEventType::TRADE_COMPLETE,
                currentLimitPrice
            );


            if (topOrderFilled) {
                engineEventQueue.emplace(
                    topOrder.idNumber,
                    0,
                    topOrder.qty,
                    EngineType::OrderEventType::FILL,
                    topOrder.getAverageFillPrice()
                );
            }
            if (incomingOrderFilled) {
                engineEventQueue.emplace(
                    orderId,
                    0,
                    incomingOrder->qty,
                    EngineType::OrderEventType::FILL,
                    incomingOrder->getAverageFillPrice()
                );
                orderIdMap[orderId] = nullptr;
            }
            if (topOrderFilled) {
                currentLimit->popHead();
                orderIdMap[topOrder.idNumber] = nullptr;
                workingBook.findNewBestPrice();
            }
        }
    }
    if (incomingOrder->getUnfilledShares() != 0) {
        incomingOrderBook.addOrderToBook(std::move(incomingOrder));
    }
}

void MatchingEngine::modifyOrderQuantity(oid_t orderId, qty_t newOrderQuantity) {
    auto &order = orderIdMap[orderId];
    // order does not exist or the updated quantity is greater than the number of unfilled shares
    if (!order || newOrderQuantity > order->getUnfilledShares()) {
        engineEventQueue.emplace(orderId, 0, 0, EngineType::OrderEventType::ORDER_REDUCE_FAIL, 0);
    } else {
        order->qty = newOrderQuantity;
        engineEventQueue.emplace(orderId, 0, newOrderQuantity, EngineType::OrderEventType::ORDER_REDUCE_SUCCESS, 0);
    }
}

// cancelled orders will get popped automatically when iteration reaches the cancelled order
// only need to set filled shares to 0
void MatchingEngine::cancelOrder(oid_t orderId) {
    auto order = orderIdMap[orderId];
    if (order) {
        const bool isBuyBook = order->type == BookType::OrderSide::BUY;
        if (isBuyBook) {
            buyBook.removeOrder(*order);
            buyBook.findNewBestPrice();
        } else {
            sellBook.removeOrder(*order);
            sellBook.findNewBestPrice();
        }

        engineEventQueue.emplace(orderId, 0, 0, EngineType::OrderEventType::CANCEL_SUCCESS, 0);
        // remove from map but keep pointer on books
        orderIdMap[orderId] = nullptr;
    }

}

std::ostream& operator<<(std::ostream &out, OrderEvent &event) {
    switch (event.type) {
        case EngineType::OrderEventType::ORDER_REDUCE_SUCCESS:
            out << "Order " << event.firstOrder << " successfully reduced to new quantity of "
            << event.numShares << "\n";
            break;
        case EngineType::OrderEventType::ORDER_REDUCE_FAIL:
            out << "Failed to reduce order quantity for order " << event.firstOrder << "\n";
            break;
        case EngineType::OrderEventType::TRADE_COMPLETE:
            out << "Order " << event.firstOrder << " and order " << event.secondOrder << " traded " << event.numShares
            << " shares at an average price of " << event.averagePrice << "\n";
            break;
        case EngineType::OrderEventType::FILL:
            out << "Order " << event.firstOrder << " filled for " << event.numShares
            << " shares at an average price of " << event.averagePrice << "\n";
            break;
        case EngineType::OrderEventType::ADDED_TO_BOOK:
            out << "Order " << event.firstOrder << " successfully added to book for " << event.numShares
            << " shares at an average price of " << event.averagePrice << "\n";
            break;
        case EngineType::OrderEventType::CANCEL_SUCCESS:
            out << "Order " << event.firstOrder << " successfully cancelled and removed from book\n";
            break;
    }
    return out;
}