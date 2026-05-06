#include<iostream>
#include<winsock2.h>
#pragma comment("lib","ws2_32.lib")

using namespace std;

int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    int fd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in target_sa;
    target_sa.sin_family=PF_INET;
    target_sa.sin_addr.S_un.S_addr=inet_addr("192.168.232.135");
    target_sa.sin_port=htons(4399);
    int check=connect(fd,(sockaddr*)&target_sa,sizeof(target_sa));
    if(check==-1)
    {
        perror("connect");
        return -1;
    }
    cout<<"connect success!"<<endl;

    // bind();
    // listen();
    // accept();

    char msg[64];
    char rmsg[64];
    while(true)
    {
        memset(msg,0,64);
        memset(rmsg,0,64);

        cout<<">";
        cin>>msg;
        send(fd,msg,strlen(msg),0);
        recv(fd,rmsg,64,0);
        cout<<"server:"<<rmsg<<endl;
    }

    closesocket(fd);
    WSACleanup();
    return 0;
}