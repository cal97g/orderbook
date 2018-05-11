//
// Created by Uncle Chu on 15/4/18.
//

#ifndef ORDERBOOK_ORDERBOOK_HPP
#define ORDERBOOK_ORDERBOOK_HPP

#include <deque>
#include <string>
#include <unordered_map>
#include <utility>
#include <memory>
#include "Order.h"
#include "PriceBucket.h"
#include "PriceBucketManager.hpp"

//#define MAX_PRICE 100000000 // 10000.0000 in fixed point format
// reasonable for most stocks except berkshire
#define MAX_PRICE 100000

// 1. need to have order struct
// 2. order book struct
template <typename PriceBucketManagerT>
class Book
{
public:
    Book(BookType t) : m_bookType{t} {}

    void addOrder( Order &order )
    {
        auto bucket = m_priceBucketManager.findBucket(order.price);
        if (bucket==nullptr)
            bucket = m_priceBucketManager.addBucket(order.price);
        bucket->addOrder(order);
    }

    void removeOrder( Order &order )
    {
        auto bucket = m_priceBucketManager.findBucket(order.price);
        bucket->removeOrder(order);
    }

    uint64_t bestPrice()
    {
        if (m_bookType == BookType::BUY)
            return m_priceBucketManager.minPrice();
        else
            return m_priceBucketManager.maxPrice();
    }
    uint32_t volumeForPricePoint( uint64_t price )
    {
        auto bucket = m_priceBucketManager.findBucket(price);
        return bucket->m_volume;
    }

private:
    BookType m_bookType;
    PriceBucketManagerT m_priceBucketManager;

//    uint64_t m_minAsk;
//    uint64_t m_maxBid;

//  Replace the above with best price. if it's a BUY book, then the best price is
//  the highest pricePoint among all the order currently in the book, i.e. best bid
//  - the highest price someone is willing to buy an item. If is the SELL book,
//  then it is the best ask, i.e. lowest of price some is willing to sell an item.

    uint64_t m_bestPrice;

};

template <typename PriceBucketManagerT>
class LimitOrderBook {
    // class implementing the facilities for a "continuous double auction"
    // see https://arxiv.org/abs/1012.0349 for general survey of limit order books.
public:
    LimitOrderBook() : m_buyBook{BookType::BUY}, m_sellBook{BookType::SELL} {}

    void addOrder(Order &order)
    {
        if ( order.side == BookType::BUY )
            m_buyBook.addOrder(order);
        else
            m_sellBook.addOrder(order);
    }

    void removeOrder( Order &order )
    {
        if ( order.side == BookType::BUY )
            m_buyBook.removeOrder(order);
        else
            m_sellBook.removeOrder(order);
    }

    uint32_t volumeForPricePoint( uint64_t price, BookType t)
    {
        if (t == BookType::BUY)
            return m_buyBook.volumeForPricePoint(price);
        else
            return m_sellBook.volumeForPricePoint(price);
    }

    uint64_t bestBid()
    {
        return m_buyBook.bestPrice();
    }

    uint64_t bestAsk()
    {
        return m_sellBook.bestPrice();
    }

private:

    Book<PriceBucketManagerT> m_buyBook;
    Book<PriceBucketManagerT> m_sellBook;

};


#endif //ORDERBOOK_ORDERBOOK_HPP