//
// Created by Martin Cooper on 8/21/20.
//
#include <iostream>
#include "Limit.h"
#include "Book.h"

Limit& Book::addOrder(Order *newOrder) {
    orderIdMap[newOrder->idNumber] = newOrder;
    auto midIndex = bookData.size() / 2;
    int price = newOrder->limitPrice;
    //check if first order in the book
    if (bestPriceIndex == -1) {
        bookData[midIndex].addOrder(newOrder);
        bookData[midIndex].setPrice(price);
        bestPriceIndex = midIndex;
        bestPriceAmount = price;
        return bookData[midIndex];
    } else {
        //assuming reasonable order here, wont go out of bounds since we allocated a reasonable size
        //TODO: error handling
        int newIndex = price - bestPriceAmount + bestPriceIndex;
        bookData[newIndex].setPrice(price);
        bookData[newIndex].addOrder(newOrder);

        if ((oType == OrderType::BUY && price > bestPriceAmount) || (oType == OrderType::SELL && price < bestPriceAmount)) {
            bestPriceIndex = newIndex;
            bestPriceAmount = price;
        }
        return bookData[newIndex];
    }
}

qty_t Book::fillSharesForOrder(Order *order, qty_t quantity) {
    auto limitLevel {findLimitLevel(order->idNumber)};
    limitLevel.partialFillOrder(order, quantity);
    return order->getUnfilledShares();
}

qty_t Book::partialCancelOrder(Order *order, qty_t quantity) {
    auto limitLevel {findLimitLevel(order->idNumber)};
    limitLevel.reduceOrderQty(order, quantity);
    return order->getUnfilledShares();
}

void Book::deleteOrder(Order *order) {
    auto const oid = order->idNumber;
    auto limitLevel {findLimitLevel(oid)};

    limitLevel.removeOrder(order);
    orderIdMap.erase(oid);
    //removed the last order at a level
    if (limitLevel.getVolume() == 0) {
        //look for new best limit price
        bool foundReplacement {false};
        if (oType == OrderType::BUY) {
            for (int i {bestPriceIndex}; i > 0; --i) {
                if (bookData[i].getVolume() > 0) {
                    bestPriceIndex = i;
                    bestPriceAmount = bookData[i].getPrice();
                    foundReplacement = true;
                    break;
                }
            }
        } else {
            for (int i {bestPriceIndex}; i < bookData.size(); ++i) {
                if (bookData[i].getVolume() > 0) {
                    bestPriceIndex = i;
                    bestPriceAmount = bookData[i].getPrice();
                    foundReplacement = true;
                    break;
                }
            }
        }
        if (!foundReplacement) {
            bestPriceAmount = -1;
            bestPriceIndex = -1;
        }
    }
}

Limit& Book::findLimitLevel(oid_t orderId) {
    Order *order {orderIdMap[orderId]};
    auto limitPrice {order->limitPrice};
    Limit& limitLevel = bookData[bestPriceIndex + limitPrice - bestPriceAmount];
    return limitLevel;
}

std::size_t Book::totalOrdersOutstanding() {
    std::size_t total {0};
    for (const Limit& level: bookData) {
        total += level.getOrderQty();
    }
    return total;
}

std::ostream& operator<< (std::ostream &out, const Book &book) {
    for (const Limit& level: book.bookData) {
        if (level.getVolume() > 0) {
            out << level << std::endl;
        }
    }
    return out;
}