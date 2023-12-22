
#include "poller.h"

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

    unsigned short sport = 8080;

    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    printf("port = %d\n", sport);
    addr.sin_port = htons(sport);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");

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
    return 0;
}

int EndPoint::Accept() {
    auto conn = accept(lfd, (sockaddr *) &current, &currentLen);
    if (conn < 0) {
        perror("accept");
        return -4;
    }

//    char strip[64] = {0};
//    char *ip = inet_ntoa(addr.sin_addr);
//    strcpy(strip, ip);
//    printf("client connect, conn:%d,ip:%s, port:%d, count:%d\n", conn, strip, ntohs(connaddr.sin_port),
//           ++count);

    clients.push_back(conn);
    // 设为非阻塞
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
            int conn;
            if (events[i].data.fd == lfd) {
                auto ret = Accept();
                if (ret < 0) {
                    perror("Accept");
                    return;
                }
            } else if (events[i].events & EPOLLIN) {
                conn = events[i].data.fd;
                if (conn < 0)
                    continue;
                char buf[1];
//                std::unique_ptr<char> &&ptr = std::make_unique<char>(100);
                auto ret = recv(conn, buf, sizeof(buf), MSG_DONTWAIT);

                printf("Read %ld\n", ret);
                if (ret == -1) {
                    perror("read");
                    continue;
                } else if (ret == 0) {
//                    printf("client close remove:%d, count:%d\n", conn, --count);
                    close(conn);
                    event = events[i];
                    epoll_ctl(efd, EPOLL_CTL_DEL, conn, &event);
                    clients.erase(std::remove(clients.begin(), clients.end(), conn), clients.end());
                }
                write(conn, buf, ret);
                memset(buf, 0, sizeof(buf));
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
    Init();
    auto rt = PreProcess();
    if (rt < 0) {
        perror("PreProcess");
        return;
    }
    Poll();


}

void EndPoint::Shutdown() {

}
