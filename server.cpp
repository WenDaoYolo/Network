#include<winsock2.h>
#include<iostream>
#pragma comment("lib","ws2_32.lib")
using namespace std;

int main()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);

    int listensocket=socket(AF_INET,SOCK_STREAM,0);
    if(listensocket==INVALID_SOCKET)
    {
        int err=WSAGetLastError();
        cout<<"error code:"<<err<<endl;
    }
    sockaddr_in server_sa;
    server_sa.sin_family=PF_INET;
    server_sa.sin_addr.S_un.S_addr=htonl(INADDR_ANY);
    server_sa.sin_port=htons(4399);
    bind(listensocket,(sockaddr*)&server_sa,sizeof(server_sa));
    listen(listensocket,5);

    cout<<"listening..."<<endl;
    int new_fd=accept(listensocket,NULL,NULL);
    if(new_fd==INVALID_SOCKET)
    {
        int err=WSAGetLastError();
        cout<<"error code:"<<err<<endl;
        return -1;
    }
    cout<<"client "<<new_fd<<" is join"<<endl;
    char recvbuffer[64];
    char sendbuffer[]="received";

    while(true)
    {
        memset(recvbuffer,0,64);
        int check=recv(new_fd,recvbuffer,64,0);
        if(check==0||check==SOCKET_ERROR)//Лђеп<=0
        {
            cout<<"client "<<new_fd<<" disconnect"<<endl;
            closesocket(new_fd);
            break;
        }
        cout<<"client "<<new_fd<<":"<<recvbuffer<<endl;
        send(new_fd,sendbuffer,strlen(sendbuffer)+1,0);
    }
    closesocket(listensocket);
    WSACleanup();
    return 0;
}
