//
// Created by Victor Dusa on 8/29/20.
//

#include <iostream>
#include "MatchingEngine.h"


void MatchingEngine::addSymbol(uint32_t sid){
    if(!mapContains(orderBooks,sid)) orderBooks->insert({sid, new OrderBook{
        new std::map<uint32_t,std::list<Order*>*>,
        new std::map<uint32_t,std::list<Order*>*,std::greater<>>}});
}

void MatchingEngine::removeSymbol(uint32_t sid){
    if(mapContains(orderBooks,sid)) orderBooks->erase(sid);
}

void MatchingEngine::placeOrder(uint32_t oid, uint32_t uid, uint32_t sid, int32_t price, int32_t size, bool isAsk) {
    if (!mapContains(orderBooks,sid)) return;
    OrderBook *book = orderBooks->at(sid);
    std::map<uint32_t,std::list<Order*>*>* askPriceBuckets = book->askPriceBuckets;
    std::map<uint32_t,std::list<Order*>*,std::greater<>>* bidPriceBuckets = book->bidPriceBuckets;
    auto *const order = new Order{oid,uid,sid,price,size,0,isAsk};
    if (tryMatchInstantly(order,askPriceBuckets,bidPriceBuckets) == order->size) return;
    if(isAsk) insertHelper(askPriceBuckets,order);
    else insertHelper(bidPriceBuckets,order);
}

void MatchingEngine::cancelOrder(uint32_t oid) {
    if(!mapContains(orderMap,oid)) return;
    Order* order = orderMap->at(oid);
    OrderBook* book = orderBooks->at(order->sid);
    if(order->isAsk) cancelHelper(book->askPriceBuckets,order);
    else cancelHelper(book->bidPriceBuckets,order);
    orderMap->erase(oid);
}

int32_t MatchingEngine::tryMatchInstantly(Order *const takerOrder,
                                             std::map<uint32_t,std::list<Order*>*>* askPriceBuckets,
                                             std::map<uint32_t,std::list<Order*>*,std::greater<>>* bidPriceBuckets) {
    int32_t remainingSize = takerOrder->size - takerOrder->filled;
    if(remainingSize == 0) return takerOrder->filled;
    const bool isAsk = takerOrder->isAsk;
    const int32_t price = takerOrder->price;
    std::list<Order*>* makerOrders;
    if(isAsk) makerOrders = matchHelper(bidPriceBuckets);
    else makerOrders = matchHelper(askPriceBuckets);
    if(makerOrders == nullptr) return takerOrder->filled;
    Order* makerOrder = makerOrders->front();
    while (remainingSize > 0 && isAsk ? makerOrder->price <= price : makerOrder->price >= price) {
        const int32_t tradeSize = std::min(remainingSize, makerOrder->size - makerOrder->filled);
        makerOrder->filled += tradeSize;
        remainingSize -= tradeSize;
        if (makerOrder->size != makerOrder->filled) break;
        orderMap->erase(makerOrder->oid);
        makerOrders->pop_front();
        if(makerOrders->empty()) {
            if(isAsk) makerOrders = matchHelper(bidPriceBuckets,makerOrder->price);
            else makerOrders = matchHelper(askPriceBuckets,makerOrder->price);
            if(makerOrders == nullptr) break;
        }
        makerOrder = makerOrders->front();
    }
    takerOrder->filled = takerOrder->size - remainingSize;
    return takerOrder->filled;
}

template<class T>
void MatchingEngine::insertHelper(T buckets, Order *order) {
    int32_t price = order->price;
    if(!mapContains(buckets,order->price))
        buckets->insert({price, new std::list<Order*>});
    buckets->at(price)->push_back(order);
}

template<class T>
void MatchingEngine::cancelHelper(T buckets, Order *order) {
    int32_t price = order->price;
    std::list<Order*>* orders = buckets->at(price);
    orders->remove(order);
    if(orders->empty()) buckets->erase(price);
}

template <class T>
std::list<Order*>* MatchingEngine::matchHelper(T buckets, int32_t price){
    buckets->erase(price);
    return buckets->empty() ? nullptr : buckets->begin()->second;
}

template <class T>
std::list<Order*>* MatchingEngine::matchHelper(T buckets){
    return buckets->empty() ? nullptr : buckets->begin()->second;
}