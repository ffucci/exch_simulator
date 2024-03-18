#include <iostream>
#include <thread>

#include "ds/lockless_queue.h"

constexpr size_t NUMBER_OF_WRITES = 4096;

int main(int argv, char** argc) {
    ff::ds::LockFreeQueue<int, NUMBER_OF_WRITES / 2> lf;

    std::thread t1 = std::thread([&lf] {
        int i = 0;
        while (!lf.full() && i < NUMBER_OF_WRITES) {
            if (i > NUMBER_OF_WRITES / 4) {
                lf.enqueue(2 * i);
            } else {
                lf.enqueue(i);
            }
            std::this_thread::yield();
            ++i;
        }
    });

    std::thread t2 = std::thread([&lf] {
        int i = 0;
        while (!lf.empty() && i < NUMBER_OF_WRITES) {
            auto element = lf.pop();
            std::cout << *element << std::endl;
            std::this_thread::yield();
            ++i;
        }
    });
    t1.join();
    t2.join();
    return 0;
}