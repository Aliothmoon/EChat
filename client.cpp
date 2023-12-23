#include <iostream>
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
#include <thread>

int main() {
    auto fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }
    struct sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    auto ret = connect(fd, (sockaddr *) &addr, sizeof addr);
    if (ret < 0) {
        perror("connect");
        return ret;
    }

    auto t = std::thread([fd]() {
        char buf[1024];
        int len = -1;
        while ((len = read(fd, buf, sizeof buf)) != 0) {
            buf[len] = 0;
            printf("Srv > %s\n", buf);
        }
    });
    char buf[1024] = {0};
    while (true) {
        scanf("%s", buf);
        int len = strlen(buf);
        send(fd, buf, len, MSG_DONTWAIT);
    }
    t.join();
    return 0;
}
