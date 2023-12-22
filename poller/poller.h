//
// Created by root on 23-12-23.
//

#ifndef ECHAT_POLLER_H
#define ECHAT_POLLER_H

#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <cstdio>
#include<algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include <memory>
// file descriptor
using FD = int;
using EPOLL_EVENT = epoll_event;
using EventList = std::vector<EPOLL_EVENT>;


void setNonblocking(int fd);


class EndPoint {
private:
    int port;
    const char *address;
//   listen file descriptor
    FD lfd = -1;
//    连接信息
    sockaddr_in current;
    socklen_t currentLen;

    std::vector<int> clients;
    EventList events{16};
    EPOLL_EVENT event;
    FD efd;
    bool running;
public:

    void Run();

    int Init();

    void Poll();

    int PreProcess();

    int Accept();


    void Shutdown();


};

#endif //ECHAT_POLLER_H
