#pragma comment("lib","ws2_32.lib")
#include<winsock2.h>
#include<iostream>
#include<cstring>
#include<thread>
#include<mutex>
using namespace std;

int fd;
bool EXIT;
mutex ExitLock;
char recvbuffer[64];
char sendbuffer[64];
sockaddr_in target_sa;

void Init()
{
    memset(sendbuffer,0,64);
    memset(recvbuffer,0,64);
    EXIT=false;

    WSADATA wsadata;
    WSAStartup(MAKEWORD(2,2),&wsadata);

    fd=socket(AF_INET,SOCK_STREAM,0);
    if(fd==INVALID_SOCKET)
    {
        cout<<"create socket,error code:"<<WSAGetLastError()<<endl;
        EXIT=true;
        return;
    }

    target_sa.sin_family=PF_INET;
    target_sa.sin_addr.S_un.S_addr=inet_addr("192.168.232.135");
    target_sa.sin_port=htons(4399);

    int check=connect(fd,(sockaddr*)&target_sa,sizeof(target_sa));
    if(check==SOCKET_ERROR)
    {
        cout<<"connect,error code:"<<WSAGetLastError()<<endl;
        closesocket(fd);
        EXIT=true;
        return;
    }
    else
        cout<<"connect success!"<<endl;
}

void Send()
{
    while(!EXIT)
    {
        memset(sendbuffer,0,64);
        cin>>sendbuffer;
        
        if(!strcmp(sendbuffer,"exit"))
        {
            lock_guard<mutex> g1(ExitLock);
            EXIT=true;   
        }
        send(fd,sendbuffer,strlen(sendbuffer),0);
    }
}

void Recv()
{
    while(true)
    {
        memset(recvbuffer,0,64);
        recv(fd,recvbuffer,64,0);
        cout<<recvbuffer<<endl;

        {
            lock_guard<mutex> g1(ExitLock);
            if(EXIT)
                break;
        }
    }
}

void loop()
{
    if(!EXIT)
    {
        thread thread_send(Send);
        thread thread_recv(Recv);
        thread_send.join();
        thread_recv.join();
    }
}

void Destroy()
{   
    cout<<"³ÌÐòÒÑÍË³ö!"<<endl;
    closesocket(fd);
    WSACleanup();
}

int main()
{
    Init();
    loop();
    Destroy();

    return 0;
}