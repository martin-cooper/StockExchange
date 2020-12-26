//
// Created by Martin Cooper on 8/21/20.
//

#ifndef STOCKEXCHANGE_TYPES_H
#define STOCKEXCHANGE_TYPES_H

#include <cstdint>

namespace BookType {
    enum class OrderSide {
        BUY,
        SELL,
    };
}

namespace EngineType {
    enum class OrderType {
        BUY,
        SELL,
        ORDER_REDUCE,
        CANCEL_ORDER,
    };

    enum class OrderEventType {
        FILL,
        CANCEL_SUCCESS,
        PARTIAL_FILL,
        ORDER_REDUCE_SUCCESS,
        ORDER_REDUCE_FAIL,
        ADDED_TO_BOOK,
    };
}
using oid_t = std::uint32_t;
using qty_t = std::uint32_t;


#endif //STOCKEXCHANGE_TYPES_H
