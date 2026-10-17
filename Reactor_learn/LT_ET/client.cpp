#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<memory.h>
#include<string.h>

void InitSocket(int& socket_fd,sockaddr_in& server_sai)
{
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1)
        throw std::runtime_error("create socket failed!");
    
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(2026);
    inet_aton("172.20.10.2",&(server_sai.sin_addr));
}

void Connect(int& socket_fd,sockaddr_in& server_sai)
{
    if(connect(socket_fd,(sockaddr*)&server_sai,sizeof(server_sai))!=-1)
        std::cout<<"connect success!"<<std::endl;
    else
        std::cout<<"connect error!"<<std::endl;

    char buffer[64];
    int write_nums;
    while(memset(buffer,0,sizeof(buffer)),true)
    {
        std::cin>>buffer;
        if(!strcmp(buffer,"exit")) break;

        int len=strlen(buffer)+1;
        send(socket_fd,&len,sizeof(len),0);
        write_nums=send(socket_fd,buffer,len,0);
        std::cout<<"current write nums:"<<write_nums<<std::endl;
    }

    close(socket_fd);
}

int main()
{
    int fd;
    sockaddr_in server_sai;
    InitSocket(fd,server_sai);
    Connect(fd,server_sai);

    return 0;
}