#include<sys/select.h>
#include<iostream>
#include<sys/socket.h>

void test1()
{
    fd_set fds, rfds;
    timeval t;
    t.tv_sec = 5;
    t.tv_usec = 0;

    int fd1 = socket(AF_INET, SOCK_STREAM, 0);
    int fd2 = socket(AF_INET, SOCK_STREAM, 0);
    std::cout << "fd1 = " << fd1 << ", fd2 = " << fd2 << std::endl;

    if (fd1 < 0 || fd2 < 0) {
        perror("socket");
        return;
    }

    FD_ZERO(&fds);
    FD_SET(fd2, &fds);   // 只监听 fd2 可读

    rfds = fds;
    int maxfd = (fd1 > fd2) ? fd1 : fd2;

    std::cout << "waiting events..." << std::endl;
    int nums = select(maxfd + 1, NULL, NULL, NULL, &t);

    if (nums < 0) {
        perror("select");
    } else if (nums == 0) {
        std::cout << "timeout, no event" << std::endl;
    } else {
        std::cout << "event on fd: ";
        if (FD_ISSET(fd2, &rfds)) std::cout << "fd2 ";
        std::cout << std::endl;
    }
}

int main()
{
    test1();

    return 0;
}