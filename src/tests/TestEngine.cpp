//
// Created by Martin Cooper on 1/7/21.
//
#include <cassert>

#include "../types.h"
#include "TestEngine.h"

extern void testProcessOrders() {
    std::queue<OrderEvent> outputEvents{};
    MatchingEngine testEngine(0, 10, outputEvents);

    testEngine.processIncomingEvent(0, 5, 20, EngineType::OrderType::BUY);
    testEngine.processIncomingEvent(1, 4, 10, EngineType::OrderType::SELL);
    testEngine.processIncomingEvent(2, 5, 10, EngineType::OrderType::SELL);
    testEngine.processIncomingEvent(3, 7, 50, EngineType::OrderType::SELL);
    testEngine.processIncomingEvent(4, 7, 27, EngineType::OrderType::SELL);

    std::vector<OrderEvent> filtered{};
    while (!outputEvents.empty()) {
        if (outputEvents.front().type != EngineType::OrderEventType::ADDED_TO_BOOK) {
            filtered.push_back(outputEvents.front());
        }
        outputEvents.pop();
    }
    std::vector<OrderEvent> expected{{
                                         {0, 10, EngineType::OrderEventType::PARTIAL_FILL, 5},
                                         {1, 10, EngineType::OrderEventType::PARTIAL_FILL, 5},
                                         {1, 10, EngineType::OrderEventType::FILL, 5},
                                         {0, 10, EngineType::OrderEventType::PARTIAL_FILL, 5},
                                         {2, 10, EngineType::OrderEventType::PARTIAL_FILL, 5},
                                         {0, 10, EngineType::OrderEventType::FILL, 5},
                                         {2, 10, EngineType::OrderEventType::FILL, 5}

                                     }};

    assert(expected.size() == filtered.size());

    for (int i = 0; i < filtered.size(); i++) {
        const auto[filteredId, filteredCount, filteredType, filteredAvgPrice] = filtered[i];
        const auto[expectedId, expectedCount, expectedType, expectedAvgPrice] = expected[i];

        assert(std::tie(filteredId, filteredCount, filteredType, filteredAvgPrice) ==
               std::tie(expectedId, expectedCount, expectedType, expectedAvgPrice));
    }
}
