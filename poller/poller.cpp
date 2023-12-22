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


using EventList = std::vector<struct epoll_event>;

int initialization() {
//    Socket
    int listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenFd < 0) {
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
    if (bind(listenFd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return -2;
    }
//    Listen
    if (listen(listenFd, 20) < 0) {
        perror("listen");
        return -3;
    }
    return listenFd;
}

void acceptor() {

}

void poll() {
}

void setNonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL);
    if (flags < 0)
        return;
    if (fcntl(fd, F_SETFL, flags |= O_NONBLOCK) < 0)
        perror("fcntl set");
}


int process() {
    auto lfd = initialization();
    if (lfd < 0) {
        return lfd;
    }
//    连接管理
    std::vector<int> clients;

//    连接信息
    sockaddr_in connaddr{};
    socklen_t len = sizeof(connaddr);

    auto efd = epoll_create1(0);
    epoll_event event{};
    event.events = EPOLLIN;
    event.data.fd = lfd;
    if (epoll_ctl(efd, EPOLL_CTL_ADD, lfd, &event) < 0) {
        perror("epoll_ctl");
        return -2;
    }
    EventList events{16};
    int count = 0;
    while (true) {

        auto readyNum = epoll_wait(efd, events.begin().base(), (int) events.size(), -1);
        if (readyNum == -1) {
            perror("epoll_wait");
            return -3;
        }
        // 对clients进行扩容
        if ((size_t) readyNum == events.size()) {
            events.resize(events.size() * 2);
        }
        for (int i = 0; i < readyNum; i++) {
            int conn;
            if (events[i].data.fd == lfd) {
                conn = accept(lfd, (struct sockaddr *) &connaddr, &len);
                if (conn < 0) {
                    perror("accept");
                    return -4;
                }
                char strip[64] = {0};
                char *ip = inet_ntoa(connaddr.sin_addr);
                strcpy(strip, ip);
                printf("client connect, conn:%d,ip:%s, port:%d, count:%d\n", conn, strip, ntohs(connaddr.sin_port),
                       ++count);

                clients.push_back(conn);
                // 设为非阻塞
                setNonblocking(conn);
                // add fd in events
                event.data.fd = conn;
                event.events = EPOLLIN;
                epoll_ctl(efd, EPOLL_CTL_ADD, conn, &event);

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
                    return -5;
                } else if (ret == 0) {
                    printf("client close remove:%d, count:%d\n", conn, --count);
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

    return 0;
}

int main() {
    process();
}