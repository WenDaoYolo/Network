#include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
using namespace std;

void test1()
{
        close(1);
        cout<<"close stdout/cout"<<endl;
}

void test2()
{
        int fd=socket(AF_INET,SOCK_STREAM,0);
        if(fd==-1){perror("create socket");return;}
        cout<<"create socket success!"<<endl;

        sockaddr_in t;
        inet_aton("192.168.232.135",&(t.sin_addr));                       //to net storage
        t.sin_port=htons(atoi("1344"));                                          //to net storage
        t.sin_family=AF_INET;

        cout<<"inet_aton after:"<<inet_ntoa(t.sin_addr)<<endl;    //to host to view
        cout<<"htons after:"<<ntohs(t.sin_port)<<endl;                //to host to view

        int check=bind(fd,(sockaddr*)&t,sizeof(t));
        if(check==-1){perror("bind socket");return;}
        cout<<"bind socket success!"<<endl;
}

int main()
{
        //test1();
        test2();

        return 0;
}
