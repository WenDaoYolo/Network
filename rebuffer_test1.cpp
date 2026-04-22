#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<thread>
#include<cstring>

void Send(int* socket_fd)
{
    char send_buffer[16]={0};
    while(true)
    {
        std::cin>>send_buffer;
        if(!strcmp(send_buffer,"exit")) break;

        send(*socket_fd,send_buffer,sizeof(send_buffer),0);
    }
}

void Connect()
{
    int socket_fd=socket(AF_INET,SOCK_STREAM,0);
    if(socket_fd==-1){perror("create socket");return;}

    struct sockaddr_in server_sa;
    server_sa.sin_family=PF_INET;
    server_sa.sin_port=htons(4399);
    inet_aton("192.168.232.135",&(server_sa.sin_addr));

    int check=connect(socket_fd,(sockaddr*)&server_sa,sizeof(server_sa));
    if(check==-1){perror("connect");return;}

    std::thread send_thread(Send,&socket_fd);
    send_thread.join();
}

int main()
{
    Connect();

    return 0;
}