//
// Created by Martin Cooper on 8/21/20.
//

#ifndef STOCKEXCHANGE_BOOK_H
#define STOCKEXCHANGE_BOOK_H

#include <vector>
#include <unordered_map>

#include "types.h"
#include "Limit.h"

class Book {
private:
    std::vector<Limit> bookData;
    OrderType::OrderSide oType;
    std::unordered_map<unsigned int, Order*> orderIdMap;
    int bestPriceAmount;
    int bestPriceIndex;
    void deleteOrder(Order *order);
    Limit& findLimitLevel(oid_t orderId);


public:
    Book(std::size_t initAllocSize, OrderType::OrderSide orderType, std::size_t initOidMapSize) :
        //Empty limit vector, initialized to initial size preventing need for constant reallocation
        bookData {std::vector<Limit> (initAllocSize, Limit {})},
        oType {orderType},
        orderIdMap {std::unordered_map<unsigned int, Order*>(initOidMapSize)},
        bestPriceAmount {-1}, bestPriceIndex {-1}
    {}
    std::vector<Limit> &getBookData(){return bookData;}
    int getBestPriceAmount() const {return bestPriceAmount;}
    int getBestPriceIndex() const {return bestPriceIndex;}
    Limit& addOrder(Order* newOrder);

    //Public interface methods
    qty_t fillSharesForOrder(Order *order, qty_t quantity);
    qty_t partialCancelOrder(Order *order, qty_t quantity);
    void fillOrder(Order *order) {deleteOrder(order);}
    void cancelOrder(Order *order) {deleteOrder(order);}
    std::size_t totalOrdersOutstanding();
    friend std::ostream& operator<< (std::ostream &out, const Book &book);
};


#endif //STOCKEXCHANGE_BOOK_H
