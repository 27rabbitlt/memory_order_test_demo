#include <thread>
#include <atomic>
#include <iostream>

std::atomic<int> flag;
int sharedVar = 0;

void increase1000000() {
    int count = 0;
    while (count < 10000000) {
        int expected = 0;
        if (flag.compare_exchange_strong(expected, 1, std::memory_order_acquire)) {
            sharedVar++;
            flag.store(0, std::memory_order_relaxed);
            count++;
        }
    }
}

int main() {
    std::cout << "lock free?: " << flag.is_lock_free() << std::endl;
    while (1) {
        sharedVar = 0;
        std::thread thread1(increase1000000);
        increase1000000();
        thread1.join();
        std::cout << "Final Result: " << sharedVar  << std::endl;
    }
    return 0;
}