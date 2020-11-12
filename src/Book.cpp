//
// Created by Martin Cooper on 8/21/20.
//
#include <iostream>
#include "Limit.h"
#include "Book.h"
#include "Order.h"

int Book::getNewLimitIndex(int newPrice) const {
    // for a buy book want to arrange high to low
    // sellers sell at highest bid and move up the book
    if (oType == BookType::OrderSide::BUY) {
        return bestPriceIndex + bestPriceAmount - newPrice;
    }
    return bestPriceIndex + newPrice - bestPriceAmount;
}

std::vector<Limit> Book::initBookData(int lowPrice, int highPrice, BookType::OrderSide orderSide) {
    auto book = std::vector<Limit>(highPrice);
    for (std::size_t i = 0; i < highPrice; ++i) {
        if (orderSide == BookType::OrderSide::BUY) {
            // first limit will be max price
            book.emplace_back(Limit(highPrice - i));
        } else {
            book.emplace_back(Limit(lowPrice + i));
        }
    }
    return book;
}

void Book::addOrderToBook(const std::shared_ptr<Order> &order) {
    orderIdMap[order->getOrderId()] = order;
    // first order
    if (bestPriceIndex == -1) {
        const auto newBestPriceIndex = bookData.size() / 2;
        bookData[newBestPriceIndex].addOrder(order);
        bestPriceIndex = newBestPriceIndex;
        bestPriceAmount = order->getLimitPrice();
    } else {
        const auto limitIndex = getNewLimitIndex(order->getLimitPrice());
        bookData[limitIndex].addOrder(order);
        if (oType == BookType::OrderSide::BUY && order->getLimitPrice() > bestPriceAmount
            || oType == BookType::OrderSide::SELL && order->getLimitPrice() < bestPriceAmount) {
            bestPriceIndex = limitIndex;
        }
    }
}