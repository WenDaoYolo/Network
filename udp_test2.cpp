#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>

using namespace std;

void test()
{
        int socket_fd=socket(AF_INET,SOCK_DGRAM,0);
        if(socket_fd==-1){perror("create socket");return;}

        sockaddr_in my_sa;
        my_sa.sin_family=PF_INET;
        my_sa.sin_port=htons(atoi("4399"));
        my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(socket_fd,(sockaddr*)&my_sa,sizeof(my_sa));

        sockaddr_in client_sa;
        socklen_t client_len=sizeof(client_sa);

        char buffer[20];
        for(int i=0;i<10;i++)
        {
                recvfrom(socket_fd,buffer,sizeof(buffer),0,(sockaddr*)&client_sa,&client_len);
                cout<<inet_ntoa(client_sa.sin_addr)<<":"<<buffer<<endl;
        }

        close(socket_fd);
}

int main()
{
        test();
        return 0;
}