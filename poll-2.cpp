#include "TcpServer.h"

int main()
{
    TcpServer tcps1(5);
    std::cout<<"listening..."<<std::endl;
    tcps1.poll_model();

    return 0;
}

/////////////////////////////////////////////////////////

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<cstring>

int main()
{
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in server_sai;
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(atoi("4399"));
    inet_aton("192.168.232.135",&(server_sai.sin_addr));

    connect(client_fd,(sockaddr*)(&server_sai),sizeof(server_sai));

    char sends[]="hello,world!";
    char recvbuffer[64]={0};

    send(client_fd,sends,strlen(sends)+1,0);
    recv(client_fd,recvbuffer,64,0);
    std::cout<<"server:"<<recvbuffer<<std::endl;
    
    send(client_fd,sends,strlen(sends)+1,0);
    recv(client_fd,recvbuffer,64,0);
    std::cout<<"server:"<<recvbuffer<<std::endl;

    send(client_fd,sends,strlen(sends)+1,0);
    recv(client_fd,recvbuffer,64,0);
    std::cout<<"server:"<<recvbuffer<<std::endl;
    
    sleep(4);
    return 0;
}