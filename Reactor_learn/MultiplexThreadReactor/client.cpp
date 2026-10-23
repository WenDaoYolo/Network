#include<sys/socket.h>
#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<memory.h>
#include<string.h>
#include<mutex>
#include<thread>
bool g_exit=false;
std::mutex check_exit;

void InitSocket(int& socket_fd,sockaddr_in& server_sai)
{
    socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1)
        throw std::runtime_error("create socket failed!");
    
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(2026);
    inet_aton("172.20.10.2",&(server_sai.sin_addr));
}

void Recv(int socket_fd)
{
    while(true)
    {
        {
            std::lock_guard<std::mutex> lg1(check_exit);
            if(g_exit) break;
        }
        char buffer[64]={0};
        int nums=recv(socket_fd,buffer,sizeof(buffer),MSG_DONTWAIT);
        if(nums>0)
            std::cout<<"server message:"<<buffer<<std::endl;
    }
}

void Connect(int& socket_fd,sockaddr_in& server_sai)
{
    if(connect(socket_fd,(sockaddr*)&server_sai,sizeof(server_sai))!=-1)
        std::cout<<"connect success!"<<std::endl;
    else
    {
        std::cout<<"connect error!"<<std::endl;
        return;
    }

    std::thread recv_thread(Recv,socket_fd);

    char buffer[64];
    int write_nums;
    while(memset(buffer,0,sizeof(buffer)),true)
    {
        std::cin>>buffer;
        if(!strcmp(buffer,"exit"))
        {
            std::lock_guard<std::mutex> lg1(check_exit);
            g_exit=true;
            break;
        }

        int len=strlen(buffer);
        int head=htonl(len);
        send(socket_fd,&head,sizeof(head),0);
        write_nums=send(socket_fd,buffer,len,0);
        std::cout<<"current write nums:"<<write_nums<<std::endl;
    }

    close(socket_fd);
    recv_thread.join();
}

int main()
{
    int fd;
    sockaddr_in server_sai;
    InitSocket(fd,server_sai);
    Connect(fd,server_sai);

    return 0;
}