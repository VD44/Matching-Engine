//
// Created by Victor Dusa on 8/29/20.
//

#ifndef UNTITLED3_MATCHINGENGINE_H
#define UNTITLED3_MATCHINGENGINE_H


#include <cstdint>
#include <unordered_map>
#include <map>
#include <list>

struct Order {
    uint32_t oid;
    uint32_t uid;
    uint32_t sid;
    int32_t price;
    int32_t size;
    int32_t filled;
    bool isAsk;
};

struct OrderBook {
    std::map<uint32_t,std::list<Order*>*>* askPriceBuckets;
    std::map<uint32_t,std::list<Order*>*,std::greater<>>* bidPriceBuckets;
};

class MatchingEngine {
private:
    std::unordered_map<uint32_t,OrderBook*>* orderBooks;
    std::unordered_map<uint32_t,Order*>* orderMap;
    int32_t tryMatchInstantly(Order *takerOrder,
             std::map<uint32_t,std::list<Order*>*>* askPriceBuckets,
             std::map<uint32_t,std::list<Order*>*,std::greater<>>* bidPriceBuckets);
    template<class T, class U>
    bool mapContains(T map, U key) { return map->find(key) != map->end(); }
    template<class T>
    void insertHelper(T buckets, Order* order);
    template<class T>
    void cancelHelper(T buckets, Order *order);
    template<class T>
    std::list<Order*>* matchHelper(T buckets);
    template<class T>
    std::list<Order*>* matchHelper(T buckets, int32_t price);
public:
    MatchingEngine() :
            orderBooks(new std::unordered_map<uint32_t,OrderBook*>),
            orderMap(new std::unordered_map<uint32_t,Order*>) {}
    void addSymbol(uint32_t sid);
    void removeSymbol(uint32_t sid);
    void placeOrder(uint32_t oid, uint32_t uid, uint32_t sid, int32_t price, int32_t size, bool isAsk);
    void cancelOrder(uint32_t oid);
};


#endif //UNTITLED3_MATCHINGENGINE_H
