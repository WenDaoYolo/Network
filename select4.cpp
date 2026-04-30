#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<cstring>

int main()
{
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in server_sai;
    server_sai.sin_family=AF_INET;
    server_sai.sin_port=htons(atoi("4399"));
    inet_aton("192.168.232.135",&(server_sai.sin_addr));

    connect(client_fd,(sockaddr*)(&server_sai),sizeof(server_sai));

    char sends[]="hello,world!";
    char recvbuffer[64];

    clock_t start=clock();
    for(int i=0;i<200000;i++)
    {
        send(client_fd,sends,strlen(sends),0);
        recv(client_fd,recvbuffer,64,0);
    }
    clock_t end=clock();

    float times=(end-start)*1.0/CLOCKS_PER_SEC;
    int val=200000/times;
    std::cout<<"time:"<<times<<"s"<<std::endl;
    std::cout<<"events per seconds:"<<val<<std::endl;
    
    return 0;
}