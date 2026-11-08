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
        // 客户端接收逻辑
    char len_buf[4];
    // 1. 先严格读 4 字节长度（使用 MSG_WAITALL 确保读满）
    int n = recv(socket_fd, len_buf, 4, MSG_DONTWAIT);
    if (n == 4) {
    int net_len = 0;
    memcpy(&net_len, len_buf, 4);
    int len = ntohl(net_len); // 网络序转主机序

    // 2. 根据长度读内容
    char* payload = new char[len + 1];
    memset(payload, 0, len + 1);
    recv(socket_fd, payload, len, MSG_WAITALL);

    std::cout << "收到服务端回复: " << payload << std::endl;
    delete[] payload;
}
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