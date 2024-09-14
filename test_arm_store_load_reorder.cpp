#include <sys/_pthread/_pthread_mutex_t.h>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <random>
#include <iostream>

std::mutex m1, m2, m3;
std::condition_variable cv1, cv2, cv3;

std::random_device rd;
std::mt19937 rg(rd());
std::uniform_int_distribution<> dist(1, 100000);

int x, y;
int r1, r2;
int ready1, ready2, ready3;

void func1() {
    while (1) {
        std::unique_lock<std::mutex> l(m1);
        cv1.wait(l, []{return ready1;});
        ready1--;
        int t = 3;
        while (dist(rg) % 10 != 3) {
            t++;
        }
        x = 1;
        r1 = y;
        std::lock_guard<std::mutex> lg(m3);
        ready3++;
        cv3.notify_one();
    }
}

void func2() {
    while (1) {
        std::unique_lock<std::mutex> l(m2);
        cv2.wait(l, []{return ready2;});
        ready2--;
        int t = 3;
        while (dist(rg) % 10 != 3) {
            t++;
        }
        y = 1;
        r2 = x;
        std::lock_guard<std::mutex> lg(m3);
        ready3++;
        cv3.notify_one();
    }
}
int main() {
    int totalCnt = 0;
    int detecCnt = 0;

    std::thread t1(func1);
    std::thread t2(func2);
    
    
    while (1) {
        x = 0;
        y = 0;
        std::unique_lock<std::mutex> l1(m1);
        std::unique_lock<std::mutex> l2(m2);
        ready1++;
        ready2++;
        cv1.notify_one();
        cv2.notify_one();
        l1.unlock();
        l2.unlock();

        std::unique_lock<std::mutex> l(m3);
        cv3.wait(l, [](){return ready3 == 2;});
        ready3 = 0;
        l.unlock();
        
        totalCnt++;
        if (r1 == 0 && r2 == 0) {
            detecCnt++;
            std::cout << "Detect " << detecCnt << " times over " << totalCnt << " iterations" << std::endl;
        }
    }
}