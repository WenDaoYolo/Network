#include<iostream>
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

        sockaddr_in server_sa;
        server_sa.sin_family=PF_INET;
        server_sa.sin_port=htons(atoi("4399"));
        inet_aton("192.168.232.135",&(server_sa.sin_addr));

        char buffer[20]="hello,world!";
        for(int i=0;i<10;i++)
                sendto(socket_fd,buffer,sizeof(buffer),0,(sockaddr*)&server_sa,sizeof(server_sa));

        close(socket_fd);
}

int main()
{
        test();
        return 0;
}