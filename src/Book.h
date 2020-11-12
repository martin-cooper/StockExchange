//
// Created by Martin Cooper on 8/21/20.
//

#ifndef STOCKEXCHANGE_BOOK_H
#define STOCKEXCHANGE_BOOK_H

#include <vector>
#include <unordered_map>

#include "types.h"
#include "Limit.h"
#include "Order.h"

class Book {
private:
    std::vector<Limit> bookData;
    BookType::OrderSide oType;
    std::unordered_map<unsigned int, std::shared_ptr<Order>> orderIdMap;
    int bestPriceAmount;
    int bestPriceIndex;

    int getNewLimitIndex(int newPrice) const;

    static std::vector<Limit> initBookData(int lowPrice, int highPrice, BookType::OrderSide orderType);


public:
    Book(int lowPrice, int initAllocSize, BookType::OrderSide orderType, std::size_t initOidMapSize) :
    // Empty limit vector, initialized to initial size preventing need for constant reallocation
            bookData{initBookData(lowPrice, initAllocSize, orderType)},
            oType{orderType},
            orderIdMap(initOidMapSize),
            bestPriceAmount{-1},
            bestPriceIndex{-1} {}

    auto begin() {
        return bookData.begin();
    }

    auto end() {
        return bookData.end();
    }

    Limit &getBestPriceLimit() {
        return bookData[bestPriceIndex];
    }

    void addOrderToBook(const std::shared_ptr<Order> &order);

};


#endif //STOCKEXCHANGE_BOOK_H
