#include <iostream>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>

#include "src/MatchingEngine.h"

std::vector<int> getRandomNumberDist(int low, int high, int qty);
void testEngine();
void volumeTest();

int main() {
    testEngine();
    return 0;
}

std::vector<int> getRandomNumberDist(int low, int high, int qty) {
    std::vector<int> v{};

    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(low, high);

    for (int i = 0; i < qty; i++) {
        v.push_back(dist(mt));
    }
    return v;
}

void testEngine() {
    std::queue<OrderEvent> outputEvents{};
    MatchingEngine testEngine(500, 200, outputEvents);
    struct test {
        oid_t id;
        int price;
        qty_t vol;
        EngineType::OrderType type;
    };
    using namespace EngineType;
    std::array<test, 3> events {{
         {0, 600, 250, OrderType::BUY},
         {1, 625, 250, OrderType::BUY},
         {2, 550, 500, OrderType::SELL},
        }};
    for (auto &item: events) {
        testEngine.handleTransaction(item.id, item.price, item.vol, item.type);
    }
    std::cout << "test\n";
}

void volumeTest() {
    std::queue<OrderEvent> outputEvents{};
    MatchingEngine testEngine(500, 200, outputEvents);


    constexpr auto samples = 200;
    auto priceDistribution = getRandomNumberDist(500, 699, samples);
    auto volDis = getRandomNumberDist(1000, 10000, samples);

    auto buyOrSellDist = getRandomNumberDist(0, 1, samples);
    std::vector<EngineType::OrderType> buyOrSell{};
    for (auto const &e: buyOrSellDist) {
        if (e == 0) {
            buyOrSell.push_back(EngineType::OrderType::BUY);
        } else {
            buyOrSell.push_back(EngineType::OrderType::SELL);
        }
    }
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < samples; i++) {
        testEngine.handleTransaction(i, priceDistribution[i], volDis[i], buyOrSell[i]);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    auto test = endTime - startTime;
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(test).count() << '\n';


}


