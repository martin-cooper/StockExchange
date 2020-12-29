//
// Created by Martin Cooper on 8/21/20.
//

#ifndef STOCKEXCHANGE_BOOK_H
#define STOCKEXCHANGE_BOOK_H

#include <vector>
#include <type_traits>
#include <iostream>
#include <optional>

#include "types.h"
#include "Limit.h"
#include "Order.h"

template<BookType::OrderSide T>
class Book {
    using limitList_t = std::vector<Limit>;
    static constexpr auto isBuyBook = T == BookType::OrderSide::BUY;

public:
    class BookIterator {
        using itType = typename std::conditional<isBuyBook, limitList_t::reverse_iterator, limitList_t::iterator>::type;
    public:
        BookIterator(limitList_t &bookData, int index) :
            it{getStartIt(bookData, index)},
            seeker{getStartIt(bookData, index)},
            end{getEndIt(bookData)} {}

        bool hasNext() {
            seekNext();
            return seeker != end;
        }

        // useSeeked is true means we have called hasNext
        Limit *getNext(bool useSeeked = true) {
            if (!useSeeked) {
                seekNext();
            }
            it = seeker;
            if (it == end) {
                return nullptr;
            }
            return &*(it);
        }

    private:
        itType it;
        // reads ahead on hasNext calls
        itType seeker;
        itType end;

        // start one before the actual start so calls to hasNext/getNext get the real start
        itType getStartIt(limitList_t &data, int index) {
            if constexpr (isBuyBook) {
                // must subtract 2 here to get correct index
                // [0,1,2,3,4,5,6,7]
                // we want index 4, array has size 8
                // go to 8 subtract one gives us array[0]
                // subtract desired index 4 array[4]=3
                // subtract one more gives us array[5]=4
                return data.rbegin() + data.size() - index - 2;
            } else {
                return data.begin() + index - 1;
            }
        }

        itType getEndIt(limitList_t &data) {
            if constexpr (isBuyBook) {
                return data.rend();
            } else {
                return data.end();
            }
        }

        void seekNext() {
            do {
                seeker++;
            } while (seeker != end && seeker->getVolume() == 0);
        }
    };

    Book(int lowPrice, int initAllocSize) :
    // Empty limit vector, initialized to initial size preventing need for constant reallocation
        bookData{},
        bestPriceAmount{std::nullopt},
        bestPriceIndex{std::nullopt} {

        for (std::size_t i = 0; i < initAllocSize; ++i) {
            bookData.emplace_back(Limit(lowPrice + i));
        }
    }

    void addOrderToBook(std::unique_ptr<Order> order) {
        const auto newPrice = order->limitPrice;
        const auto limitIndex = getNewLimitIndex(order->limitPrice);

        bookData[limitIndex].addOrder(std::move(order));

        if constexpr (isBuyBook) {
            if (!bestPriceAmount.has_value() || (bestPriceAmount.has_value() && newPrice > bestPriceAmount)) {
                bestPriceAmount = newPrice;
                bestPriceIndex = limitIndex;
            }
        } else {
            if (!bestPriceAmount.has_value() || (bestPriceAmount.has_value() && newPrice < bestPriceAmount)) {
                bestPriceAmount = newPrice;
                bestPriceIndex = limitIndex;
            }
        }
    }

    void findNewBestPrice() {
        auto it = iterator();
        if (it.hasNext()) {
            const Limit *bestLimit = it.getNext();
            bestPriceAmount = bestLimit->getPrice();
            bestPriceIndex = bestPriceAmount.value() - bookData[0].getPrice();
        } else {
            bestPriceAmount = std::nullopt;
            bestPriceIndex = std::nullopt;
        }
    }

    void removeOrder(Order &order) {
        const auto beginIndex = getNewLimitIndex(order.limitPrice);
        Limit &limit = bookData[beginIndex];
        limit.clearOrder(order);
    }

    std::optional<int> getBestPriceIndex() const {
        return bestPriceIndex;
    }

    std::optional<unsigned int> getBestPriceAmount() const {
        return bestPriceAmount;
    }

    BookIterator iterator() {
        return BookIterator(bookData, bestPriceIndex.value_or(0));
    }

private:

    limitList_t bookData;
    std::optional<int> bestPriceAmount;
    std::optional<unsigned int> bestPriceIndex;

    int getNewLimitIndex(int newPrice) const {
        return newPrice - bookData[0].getPrice();
    }


};


#endif //STOCKEXCHANGE_BOOK_H
