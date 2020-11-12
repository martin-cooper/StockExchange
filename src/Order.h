//
// Created by Martin Cooper on 10/21/20.
//

#ifndef STOCKEXCHANGE_ORDER_H
#define STOCKEXCHANGE_ORDER_H

#include <ctime>
#include "types.h"

class Order {
private:
    const oid_t idNumber;
    qty_t qty;
    const int limitPrice;
    qty_t sharesFilled;
    // Sum of numFilled * price, prevents loss of data due to floating point arithmetic
    int cumulativeFillPrice;
    const time_t orderTime;

public:
    Order(oid_t id, qty_t qty, int limitPrice) :
            idNumber{id},
            qty{qty},
            limitPrice{limitPrice},
            sharesFilled{0},
            cumulativeFillPrice{0},
            orderTime{time(nullptr)} {}

    qty_t getUnfilledShares() const {
        return qty - sharesFilled;
    }

    qty_t getFilledShares() const {
        return sharesFilled;
    }

    qty_t getOrderQuantity() const {
        return qty;
    }

    oid_t getOrderId() const {
        return idNumber;
    }

    int getLimitPrice() const {
        return limitPrice;
    }

    qty_t fillSharesAtPrice(qty_t numShares, int fillPrice) {
        qty -= numShares;
        cumulativeFillPrice += numShares * fillPrice;
        return qty;
    }
};


#endif //STOCKEXCHANGE_ORDER_H
