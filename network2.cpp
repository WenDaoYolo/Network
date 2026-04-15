#include<iostream>
#include<memory.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
using namespace std;

void test1()
{
        struct sockaddr_in sa_in;

        sa_in.sin_family=AF_INET;
        sa_in.sin_port=1234;
        sa_in.sin_addr.s_addr=1;

        in_addr ia={321};
        struct sockaddr_in sa_in2={PF_INET,1111,{2}};

        cout<<"port1:"<<sa_in.sin_port<<" addr2:"<<sa_in.sin_addr.s_addr<<endl;
        cout<<"addr2:"<<ia.s_addr<<endl;
        cout<<"port2:"<<sa_in2.sin_port<<" addr3:"<<sa_in2.sin_addr.s_addr<<endl;
}

void test2()
{
        in_addr t1;
        hostent* h=gethostbyname("www.baidu.com");
        memcpy(&(t1.s_addr),h->h_addr_list[0],h->h_length);
        cout<<"baidu ip:"<<inet_ntoa(t1)<<endl;
}

int main()
{
        test1();
        test2();

        return 0;
}