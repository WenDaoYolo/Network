#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>
#define BUFFER_SIZE 64
using namespace std;

void RW(int* socket_fd)
{
        char send_buffer[BUFFER_SIZE]={0};
        char recv_buffer[BUFFER_SIZE]={0};
        while(true)
        {
                cout<<">";
                cin>>send_buffer;
                send(*socket_fd,send_buffer,strlen(send_buffer)+1,0);
                recv(*socket_fd,recv_buffer,sizeof(recv_buffer),MSG_DONTWAIT);
                cout<<"server:"<<recv_buffer<<endl;

                if(!strcmp(send_buffer,"exit")){cout<<"Program Exit!"<<endl;return;}
        }
}

void Connect()
{
        int socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if(socket_fd==-1){perror("create socket");return;}

        sockaddr_in server_sa;
        server_sa.sin_family=PF_INET;
        server_sa.sin_port=htons(4399);
        server_sa.sin_addr.s_addr=htonl(INADDR_ANY);

        int check=connect(socket_fd,(sockaddr*)&server_sa,sizeof(server_sa));
        if(check==-1){perror("conncet");return;}

        thread rw_thread(RW,&socket_fd);
        rw_thread.join();
        close(socket_fd);
}

int main()
{
        Connect();
        return 0;
}