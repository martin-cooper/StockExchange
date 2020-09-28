//
// Created by Martin Cooper on 8/21/20.
//

#ifndef STOCKEXCHANGE_TYPES_H
#define STOCKEXCHANGE_TYPES_H

namespace OrderType {
    enum OrderSide {
        BUY,
        SELL,
    };
}

namespace EngineType {
    enum OrderType {
        BUY,
        SELL,
        ORDER_REDUCE,
        CANCEL_ORDER,
    };

    enum OrderEventType {
        FILL,
        CANCEL_RESULT,
        PARTIAL_FILL,
        ORDER_REDUCE_RESULT,
        ADDED_TO_BOOK,
    };
}

using oid_t = std::uint32_t;
using qty_t = std::uint32_t;
#endif //STOCKEXCHANGE_TYPES_H
