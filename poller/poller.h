#ifndef ECHAT_POLLER_H
#define ECHAT_POLLER_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstdio>
#include <algorithm>
#include <functional>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include <memory>
#include "../executor/pool.h"
#include "../map/SyncMap.h"
#include "../handler/user.h"
#include "../list/list.h"
// file descriptor
using FD = int;
using EPOLL_EVENT = epoll_event;
using EventList = std::vector<EPOLL_EVENT>;


void setNonblocking(int fd);


class EndPoint {
private:
    unsigned short port{8080};
    const char *address{"0.0.0.0"};
//   listen file descriptor
    FD lfd = -1;
//    连接信息
    sockaddr_in current{};
    socklen_t currentLen{};

    std::vector<FD> clients;

    EventList events{16};
    EPOLL_EVENT event{};
    FD efd{};
    bool running{};

//    Tasks dispatcher
    ThreadPool pool{10};

    SyncMap<int, SyncList<User> *> online;


public:

    void Run();

    int Init();

    void Poll();

    int PreProcess();

    void Process(EPOLL_EVENT &ev);

    int Accept();

    void Shutdown();


};

#endif //ECHAT_POLLER_H
