#include<iostream>
#include<sys/socket.h>

void test()
{
    int socket_fd=socket(AF_INET,SOCK_STREAM,0);
    int buffersize;
    socklen_t buffersize_len=sizeof(buffersize);
    
    getsockopt(socket_fd,SOL_SOCKET,SO_SNDBUF,&buffersize,&buffersize_len);
    std::cout<<"send buffer default size:"<<buffersize<<std::endl;
    getsockopt(socket_fd,SOL_SOCKET,SO_RCVBUF,&buffersize,&buffersize_len);
    std::cout<<"recv buffer default size:"<<buffersize<<std::endl;

    int newsize1=6000,newsize2=6000;
    setsockopt(socket_fd, SOL_SOCKET,SO_SNDBUF,&newsize1,sizeof(newsize1));
    setsockopt(socket_fd, SOL_SOCKET,SO_RCVBUF,&newsize2,sizeof(newsize2));
    std::cout<<"set value 6000,send buffer:"<<newsize1<<std::endl;
    std::cout<<"set value 6000,recv buffer:"<<newsize2<<std::endl;
}

int main()
{
    test();

    return 0;
}