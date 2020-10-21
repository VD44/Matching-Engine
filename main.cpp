#include <chrono>
#include <sstream>
#include <iostream>
#include "MatchingEngine.h"

int main() {
    auto* m = new MatchingEngine();
    m->addSymbol(1);
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 2000000; i+=10) {
        m->placeOrder(i+0,2, 1, 8, 5,false);
        m->placeOrder(i+1,2, 1, 5, 5,false);
        m->placeOrder(i+2,1, 1, 8, 4,true);
        m->placeOrder(i+3,1, 1, 12, 1,true);
        m->placeOrder(i+4,3, 1, 9, 5,true);
        m->placeOrder(i+5,4, 1, 8, 4,false);
        m->placeOrder(i+6,3, 1, 7, 7,true);
        m->placeOrder(i+7,4, 1, 6, 2,false);
        m->placeOrder(i+8,5, 1, 8, 8,false);
        m->placeOrder(i+9,5, 1, 13, 2,false);
    }
    auto stop = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = stop-start;
    std::cout << diff.count() << std::endl;
    return 0;
}
