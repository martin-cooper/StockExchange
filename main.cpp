#include <iostream>
#include <array>
#include <chrono>
#include <random>
#include <algorithm>

#include "src/Engine.h"

std::vector<int> getRandomNumberDist(int low, int high, int qty);

int main() {
    std::queue<OrderEvent> outputEvents{};
    Engine testEngine(500, 200, outputEvents);

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

    std::vector<int> idDist{};
    for (int i = 0; i < samples; i++) {
        idDist.push_back(i);
    }
    auto rd = std::random_device{};
    auto rng = std::default_random_engine{rd()};
    std::shuffle(idDist.begin(), idDist.end(), rng);
    auto removeStartTime = std::chrono::high_resolution_clock::now();

    for (int id: idDist) {
        testEngine.cancelOrder(id);
    }
    auto newEndTime = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(newEndTime - removeStartTime).count() << '\n';

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


