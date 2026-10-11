#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<memory.h>

void InitSocket(int& fd)
{
    fd=socket(AF_INET,SOCK_DGRAM,0);
    if(fd==-1)
        throw std::runtime_error("create socket failed!");

    sockaddr_in sai;
    sai.sin_family=AF_INET;
    sai.sin_port=htons(2026);
    sai.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(fd,(sockaddr*)&sai,sizeof(sai));
}

void Recvfrom(int& fd)
{
    char buffer[64];
    sockaddr_in csai;
    socklen_t len=sizeof(csai);

    while(memset(buffer,0,sizeof(buffer)),true)
    {
        std::cout<<"waiting message..."<<std::endl;
        int read_nums=recvfrom(fd,buffer,sizeof(buffer),0,(sockaddr*)&csai,&len);
        if(read_nums==-1)
            perror("read:");
        
        if(!strcmp(buffer,"exit"))
        {
            std::cout<<"server will be down by remote..."<<std::endl;
            sleep(2);
            break;
        }

        std::cout<<"from:"<<inet_ntoa(csai.sin_addr)<<std::endl;
        std::cout<<"current read nums:"<<read_nums<<std::endl;
        std::cout<<"content:"<<buffer<<std::endl;
    }
}

void Loop(int& fd)
{
    sockaddr_in tmp;
    socklen_t len_ptr=sizeof(tmp);
    Recvfrom(fd);    
    close(fd);
}

int main()
{
    int listen_fd;
    InitSocket(listen_fd);
    Loop(listen_fd);

    return 0;
}