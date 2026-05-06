#include<winsock2.h>
#include<iostream>
#pragma comment("lib","ws2_32.lib")
using namespace std;

int main()
{
    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);

    int fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    {
        int err=WSAGetLastError();
        cout<<"error code:"<<err<<endl;
    }
    sockaddr_in server_sa;
    server_sa.sin_family=PF_INET;
    server_sa.sin_addr.S_un.S_addr=inet_addr("192.168.232.135");
    server_sa.sin_port=htons(4399);

    int check=connect(fd,(sockaddr*)&server_sa,sizeof(server_sa));
    if(check==SOCKET_ERROR)
    {
        int err=WSAGetLastError();
        cout<<"error code:"<<err<<endl;
        closesocket(fd);
        return -1;
    }
    cout<<"conncet success!"<<endl;
    char recvbuffer[64];
    char sendbuffer[64];

    while(true)
    {
        memset(recvbuffer,0,64);
        memset(sendbuffer,0,64);
        cout<<">";
        cin>>sendbuffer;
        if(!strcmp(sendbuffer,"exit"))
        {
            cout<<"program exit!"<<endl;
            closesocket(fd);
            break;
        }
        send(fd,sendbuffer,strlen(sendbuffer)+1,0);
        recv(fd,recvbuffer,64,0);
        cout<<"server:"<<recvbuffer<<endl;
    }
    WSACleanup();
    return 0;
}