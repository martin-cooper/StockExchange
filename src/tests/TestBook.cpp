//
// Created by Martin Cooper on 1/2/21.
//

#include "TestBook.h"


extern void testAddOrderToBook() {
    Book<BookType::OrderSide::BUY> buyBook{0, 10};
    buyBook.addOrderToBook(std::make_unique<Order>(0, 25, 3, BookType::OrderSide::BUY));

    assert(buyBook.getBestPriceAmount().value_or(-1) == 3);
    assert(buyBook.getBestPriceIndex().value_or(-1) == 3);

    Book<BookType::OrderSide::SELL> sellBook{0, 10};
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 8, BookType::OrderSide::BUY));
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 5, BookType::OrderSide::BUY));
    assert(sellBook.getBestPriceAmount().value() == 5);
}

extern void testIterateThroughBook() {
    Book<BookType::OrderSide::BUY> buyBook{0, 10};

    buyBook.addOrderToBook(std::make_unique<Order>(0, 33, 8, BookType::OrderSide::BUY));
    buyBook.addOrderToBook(std::make_unique<Order>(0, 33, 5, BookType::OrderSide::BUY));
    buyBook.addOrderToBook(std::make_unique<Order>(0, 33, 9, BookType::OrderSide::BUY));
    buyBook.addOrderToBook(std::make_unique<Order>(0, 33, 2, BookType::OrderSide::BUY));

    auto it = buyBook.iterator();
    assert(it.getNext(false)->getPrice() == 9);
    assert(it.getNext(false)->getPrice() == 8);
    assert(it.getNext(false)->getPrice() == 5);
    assert(it.getNext(false)->getPrice() == 2);
    assert(it.getNext(false) == nullptr);

    Book<BookType::OrderSide::SELL> sellBook{0, 10};
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 8, BookType::OrderSide::BUY));
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 5, BookType::OrderSide::BUY));
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 9, BookType::OrderSide::BUY));
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 2, BookType::OrderSide::BUY));
    sellBook.addOrderToBook(std::make_unique<Order>(0, 33, 2, BookType::OrderSide::BUY));

    auto newIt = sellBook.iterator();
    assert(newIt.getNext(false)->getPrice() == 2);
    assert(newIt.getNext(false)->getPrice() == 5);
    assert(newIt.getNext(false)->getPrice() == 8);
    assert(newIt.getNext(false)->getPrice() == 9);
    assert(newIt.getNext(false) == nullptr);

}