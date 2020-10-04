#include <iostream>
#include <vector>
#include <chrono>
#include <random>

#include "src/Limit.h"
#include "src/Book.h"
#include "src/Engine.h"
void testLimit();
void testBook();
void testEngine();
int main() {
    testBook();
    testEngine();
    return 0;
}

void testLimit() {
    Limit testLimit {};
    testLimit.setPrice(150);
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist {1,250};
    std::uniform_int_distribution<std::mt19937::result_type> randIndex {0, 24};

    std::vector<Order*> orders {std::vector<Order*> {}};
    for (std::size_t i {0}; i < 25; ++i) {
        qty_t randQty {dist(rng)};
        Order *order {new Order {static_cast<oid_t>(i), randQty, 150}};
        testLimit.addOrder(order);
        orders.push_back(order);
    }
    std::cout << testLimit << std::endl;

    auto ind {randIndex(rng)};
    auto randOrder {orders[ind]};
    std::cout << "Rand index: " << ind << std::endl;

    testLimit.partialFillOrder(randOrder, randOrder->qty / 2);

    std::cout << testLimit << std::endl;


    ind = randIndex(rng);
    randOrder = orders[ind];
    std::cout << "Rand index: " << ind << std::endl;
    testLimit.removeOrder(randOrder);

    std::cout << testLimit << std::endl;
}

void testBook() {
    Book testBook {Book {30, OrderType::BUY, 400}};
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist {1,250};
    std::uniform_int_distribution<std::mt19937::result_type> prices {1, 5};
    std::uniform_int_distribution<std::mt19937::result_type> randIndex {0, 24};
    
    for (std::size_t i {0}; i < 25; ++i) {
        qty_t randQty {dist(rng)};
        Order *order {new Order {static_cast<oid_t>(i), randQty, static_cast<int>(prices(rng))}};
        testBook.addOrder(order);
    }
    std::cout << testBook << std::endl;
}

void testEngine() {
    Engine testEngine {Engine {500}};
    auto& sellBook {testEngine.getSellBook()};
    sellBook.addOrder(new Order {1, 267, 140});

    auto results {testEngine.processIncomingOrder(2, 140, 69, EngineType::BUY)};
    for (auto &result: *results) {
        std::cout << "OrderId: " << result.orderId << ", Type: " << result.type << ", Average price: " <<
        result.averagePrice << ", Num shares: " << result.numShares << ", timestamp: " << result.timestamp <<
        std::endl;
    }
}
