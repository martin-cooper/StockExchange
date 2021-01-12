//
// Created by Martin Cooper on 1/1/21.
//

#include <cassert>
#include <utility>

#include "TestLimit.h"

extern void testAddOrder() {
    Limit testLimit(0);
    testLimit.addOrder(std::make_unique<Order>(0, 500, 0, BookType::OrderSide::BUY));
    assert(testLimit.getOrderQty() == 1);
    assert(testLimit.hasOrders());
    assert(testLimit.getVolume() == 500);
}

extern void testFillHead() {
    Limit testLimit(0);
    testLimit.addOrder(std::make_unique<Order>(0, 500, 0, BookType::OrderSide::BUY));

    auto firstFillResult = testLimit.fillSharesForHead(250);
    auto sharesFilled = std::get<0>(firstFillResult);
    auto orderFilled = std::get<1>(firstFillResult);

    assert(sharesFilled == 250);
    assert(orderFilled == false);
    assert(testLimit.getVolume() == 250);
    assert(testLimit.hasOrders());

    auto secondFillResult = testLimit.fillSharesForHead(250);
    sharesFilled = std::get<0>(secondFillResult);
    orderFilled = std::get<1>(secondFillResult);

    assert(sharesFilled == 250);
    assert(orderFilled == true);
    assert(testLimit.getOrderQty() == 0);
    assert(testLimit.getVolume() == 0);
    assert(testLimit.hasOrders() == false);
}

extern void testCancelOrder() {
    Limit testLimit(0);
    auto testOrder{std::make_unique<Order>(0, 500, 0, BookType::OrderSide::BUY)};
    const auto testOrderView{testOrder.get()};
    testLimit.addOrder(std::move(testOrder));

    testLimit.clearOrder(*testOrderView);

    assert(testLimit.hasOrders() == false);
    assert(testLimit.getOrderQty() == 0);
    assert(testLimit.getVolume() == 0);
}
