#include <queue>

#include <engine/Book.h>
#include <engine/types.h>
#include <engine/MatchingEngine.h>
#include <iostream>

int main() {
    std::cout << "Test\n";

    int lowPrice = 0;
    int highPrice = 0;
    oid_t currentOrderId = 0;
    std::string command{};

    std::cout << "Input the price range for your instrument\n";
    std::cin >> lowPrice;
    std::cin >> highPrice;

    std::cout << "Created engine accepting prices from " << lowPrice << " to " << highPrice << "\n";
    std::cout << "Type (b)uy or (s)ell followed by a price and quantity\n"
                << " or (c)ancel followed by an order id\n"
                << " or (r)educe followed by an order id and a new order quantity\n";

    std::queue<OrderEvent> events{};
    MatchingEngine engine{lowPrice, highPrice, events};

    while (true) {
        std::cin >> command;
        if (command == "exit") {
            break;
        }
        int price;
        qty_t qty;
        oid_t orderId;

        EngineType::OrderType type;
        if (command == "b") {
            std::cin >> price;
            std::cin >> qty;
            type = EngineType::OrderType::BUY;
            engine.processIncomingEvent(currentOrderId++, price, qty, type);
        } else if (command == "s") {
            std::cin >> price;
            std::cin >> qty;
            type = EngineType::OrderType::SELL;
            engine.processIncomingEvent(currentOrderId++, price, qty, type);
        } else if (command == "c") {
            std::cin >> orderId;
            type = EngineType::OrderType::CANCEL_ORDER;
            engine.processIncomingEvent(orderId, 0, 0, type);
        } else {
            std::cin >> orderId;
            std::cin >> qty;
            type = EngineType::OrderType::ORDER_REDUCE;
            engine.processIncomingEvent(orderId, 0, qty, type);
        }


        while (!events.empty()) {
            auto &frontEvent = events.front();
            std::cout << frontEvent;
            events.pop();
        }

    }

    return 0;
}






