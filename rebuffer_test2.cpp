#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<thread>
#include<cstring>
#include<unistd.h>
#include<chrono>

void Recv(int* socket_fd)
{
    char recv_buffer[16]={0};
    while(true)
    {
        sleep(10);
        int check=recv(*socket_fd,recv_buffer,sizeof(recv_buffer),0);
        if(check>0) std::cout<<"client:"<<recv_buffer<<std::endl;
    }
}

void Wait()
{
    int socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){perror("create socket");return;}

    struct sockaddr_in my_sa;
    my_sa.sin_family=PF_INET;
    my_sa.sin_port=htons(4399);
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);

    bind(socket_fd,(sockaddr*)&(my_sa),sizeof(my_sa));
    listen(socket_fd,5);
    int new_fd=accept(socket_fd,NULL,NULL);

    std::thread recv_thread(Recv,&new_fd);
    recv_thread.join();
}

int main()
{
    Wait();

    return 0;
}