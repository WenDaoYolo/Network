#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<netdb.h>
#include<memory.h>

void create_socket()
{
    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==-1)
    {
        std::cout<<"socket error!,code:"<<errno<<std::endl;
        return;
    }
                                    //current byte order: small
                                    //low                           heigh
    uint16_t u1=256;                //00000000 00000001
    uint32_t u2=256;                //00000000 00000001 00000000 00000000
    std::cout<<htons(u1)<<std::endl;//00000001 00000000                   =1
    std::cout<<htonl(u2)<<std::endl;//00000000 00000000 00000001 00000000 =2^16=65536
    //while(true);
    close(fd);
}

void define_socket_pack()
{
    sockaddr_in socki;
    socki.sin_family=AF_INET;
    socki.sin_port=htons(2026);
    socki.sin_addr.s_addr=htonl(INADDR_ANY);
}

void GetBaiduIP()
{
    hostent* ptr=gethostbyname("www.baidu.com");
    in_addr ipv4;
    memcpy(&(ipv4.s_addr),ptr->h_addr_list[0],ptr->h_length);
    std::cout<<"baidu ip:"<<inet_ntoa(ipv4)<<std::endl;
}

void ConnectBaidu21()
{
    int my_fd=socket(AF_INET,SOCK_STREAM,0);
    if(my_fd==-1)
    {
        std::cout<<"socket(..).. failed,error code"<<errno<<std::endl;
        return;
    }

    sockaddr_in server_sai;
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(21);
    inet_aton("111.63.65.247",&(server_sai.sin_addr));
    int check=connect(my_fd,(sockaddr*)&server_sai,sizeof(server_sai));
    
    if(!check)
    {
        std::cout<<"connect success!"<<std::endl;
        close(my_fd);
        std::cout<<"close fd"<<std::endl;
    }
    else
        std::cout<<"connect failed!"<<std::endl;
}

int main()
{
    create_socket();
    define_socket_pack();
    GetBaiduIP();
    ConnectBaidu21();

    return 0;
}