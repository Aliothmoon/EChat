
#include <iostream>
#include "poller.h"
#include "../list/list.h"

void setNonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        return;
    if (fcntl(fd, F_SETFL, flags |= O_NONBLOCK) < 0)
        perror("fcntl set");
}


int EndPoint::Init() {
    //    Socket
    auto fd = this->lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    unsigned short sport = port;

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    printf("Address : %s:%d\n", address, sport);
    addr.sin_port = htons(sport);
    addr.sin_addr.s_addr = inet_addr(address);

// Bind
    if (bind(fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return -2;
    }
//    Listen
    if (listen(fd, 20) < 0) {
        perror("listen");
        return -3;
    }

    running = true;

    return 0;
}

int EndPoint::Accept() {
    auto conn = accept(lfd, (sockaddr *) &current, &currentLen);
    if (conn < 0) {
        perror("accept");
        return -4;
    }

    char strip[64] = {0};
    char *ip = inet_ntoa(current.sin_addr);
    strcpy(strip, ip);
    printf("Client connect, conn:%d,ip:%s, port:%d\n", conn, strip, ntohs(current.sin_port));

    char buf[512] = {0};
    sprintf(buf, "Welcome to EChat: %d\n", conn);
    send(conn, buf, strlen(buf), MSG_DONTWAIT);

    clients.push_back(conn);
    // setNonblocking fd
    setNonblocking(conn);
    // add fd in events
    event.data.fd = conn;
    event.events = EPOLLIN;
    epoll_ctl(efd, EPOLL_CTL_ADD, conn, &event);
    return conn;
}

void EndPoint::Poll() {
    while (running) {

        auto readyNum = epoll_wait(efd, events.begin().base(), (int) events.size(), -1);
        if (readyNum == -1) {
            perror("epoll_wait");
            continue;
        }
        // 对clients进行扩容
        if ((size_t) readyNum == events.size()) {
            events.resize(events.size() * 2);
        }
        for (int i = 0; i < readyNum; i++) {
            if (events[i].data.fd == lfd) {
//                处理连接事件
                auto ret = Accept();
                if (ret < 0) {
                    perror("Accept");
                    continue;
                }
            } else if (events[i].events & EPOLLIN) {

                Process(events[i]);
//                处理 读事件
            }
        }
    }
}

int EndPoint::PreProcess() {

    if (lfd < 0) {
        printf("FD not init");
        return -1;
    }

    this->efd = epoll_create1(0);
    event.events = EPOLLIN;
    event.data.fd = lfd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &event) < 0) {
        perror("epoll_ctl");
        return -2;
    }
    return 0;
}

void EndPoint::Run() {
    auto status = Init();
    if (status < 0) {
        perror("Init");
        return;
    }
    status = PreProcess();
    if (status < 0) {
        perror("PreProcess");
        return;
    }
    Poll();


}

void EndPoint::Process(EPOLL_EVENT &ev) {
    int conn = ev.data.fd;
    if (conn < 0)
        return;

    char buf[2048] = {0};
    auto len = recv(conn, buf, sizeof buf, MSG_DONTWAIT);
    if (len == -1) {
        perror("read");
        return;
    } else if (len == 0) {
        printf("Client close :%d\n", conn);
        close(conn);
        epoll_ctl(efd, EPOLL_CTL_DEL, conn, &ev);
        clients.erase(std::remove(clients.begin(), clients.end(), conn), clients.end());
    } else {
        for (const auto &item: clients) {
            if (item != conn) {
                char head[10] = {0}, real[2048 + 10] = {0};
                sprintf(head, "%d:", conn);
                auto sz = strlen(strcpy(real, head));
                strcpy(real + sz, buf);
                printf("Send %d -> %d %s", conn, item, real);
                std::cout << std::endl;
                send(item, real, strlen(real), MSG_DONTWAIT);
            }
        }
    }


}

void EndPoint::Shutdown() {
    this->running = false;
    pool.shutdown();
}


