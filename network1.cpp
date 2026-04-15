include<iostream>
#include<sys/socket.h>
#include<unistd.h>
#include<netinet/in.h>
using namespace std;

void test1()
{
        int socket_fd=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
        if(socket_fd!=-1)
                cout<<"Create Socket success!"<<endl;
}

void test2()
{
        while(true)
        {
                int socket_fd=socket(PF_INET,SOCK_STREAM,0);
                cout<<"socket  "<<socket_fd<<endl;
                if(socket_fd==-1)
                {
                        perror(":");
                        return;
                }
        }
        uint16_t t1;
        uint32_t t2;
        unsigned int ui;
        unsigned short us;
}

void test3()
{                                  				 //low          	   high
        uint16_t a=256;            			 //00000001 00000000    host
        cout<<htons(a)<<endl;      		 //00000000 00000001    net
                                   				 //result:1
        cout<<a<<endl;
}

int main()
{
        test1();
        test2();
        test3();

        return 0;
}
