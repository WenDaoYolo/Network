#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<memory.h>
#include<string.h>

void InitSocket(int& socket_fd,sockaddr_in& server_sai)
{
    socket_fd=socket(AF_INET,SOCK_DGRAM,0);
    if(socket_fd==-1)
        throw std::runtime_error("create socket failed!");
    
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(2026);
    inet_aton("172.20.10.2",&(server_sai.sin_addr));
}

void Sendto(int& socket_fd,sockaddr_in& server_sai)
{
    char buffer[64];
    int write_nums;
    while(memset(buffer,0,sizeof(buffer)),true)
    {
        std::cin>>buffer;
        write_nums=sendto(socket_fd,buffer,strlen(buffer)+1,0,(sockaddr*)&server_sai,sizeof(server_sai));
        std::cout<<"current write nums:"<<write_nums<<std::endl;
        
        if(!strcmp(buffer,"exit"))
            break;
    }

    close(socket_fd);
}

int main()
{
    int fd;
    sockaddr_in server_sai;
    InitSocket(fd,server_sai);
    Sendto(fd,server_sai);

    return 0;
}