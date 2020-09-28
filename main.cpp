#include <iostream>
#include <vector>
#include <chrono>
#include <random>

#include "src/Limit.h"
#include "src/Book.h"
#include "src/Engine.h"

int main() {
    Engine TSLA{Engine{10}};
    Book& bruh{TSLA.getSellBook()};
    bruh.addOrder(new Order{
        0,
        15,
        23,
    });
    auto yolo{TSLA.processIncomingOrder(1, 24, 13, EngineType::BUY)};
    return 0;
}
