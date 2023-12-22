#include <iostream>
#include "executor/pool.h"

int main() {
    ThreadPool pool(10);
    // 初始化线程池
    pool.init();
    std::vector<std::future<void>> ts;
    for (int i = 0; i < 100; ++i) {
        auto it = pool.submit([i]() {
            printf("%d\n", i);
            return;
        });
        ts.push_back(std::move(it));
    }
    for (auto &item: ts) {
        item.get();
    }
    pool.shutdown();
    return 0;
}
