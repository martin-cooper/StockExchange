//
// Created by Martin Cooper on 10/21/20.
//

#ifndef STOCKEXCHANGE_ORDER_H
#define STOCKEXCHANGE_ORDER_H

#include "types.h"

struct Order {
    const oid_t idNumber;
    const int limitPrice;
    qty_t qty;
    qty_t sharesFilled;
    // Sum of numFilled * price, prevents loss of data due to floating point arithmetic
    int cumulativeFillPrice;
    BookType::OrderSide type;

    Order(oid_t id, qty_t qty, int limitPrice, BookType::OrderSide side) :
        idNumber{id},
        qty{qty},
        limitPrice{limitPrice},
        type{side},
        sharesFilled{0},
        cumulativeFillPrice{0} {
    }

    qty_t getUnfilledShares() const {
        return qty - sharesFilled;
    }

    double getAverageFillPrice() const {
        return 1.0 * cumulativeFillPrice / sharesFilled;
    }

    qty_t fillSharesAtPrice(qty_t numShares, int fillPrice) {
        sharesFilled += numShares;
        cumulativeFillPrice += numShares * fillPrice;
        return qty;
    }
};


#endif //STOCKEXCHANGE_ORDER_H
