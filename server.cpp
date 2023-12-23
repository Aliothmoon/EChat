#include <iostream>
#include "executor/pool.h"
#include "poller/poller.h"

int main() {
    EndPoint point;
    point.Run();
    point.Shutdown();
    return 0;
}
