#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>

#define BUFFER_SIZE 64
using namespace std;
bool Exit=false;

void Send(int* socket_fd,sockaddr_in* sa,socklen_t sa_len)
{
        char buffer[BUFFER_SIZE];
        while(!Exit)
        {
                cin>>buffer;
                if(!strcmp(buffer,"exit")) Exit=true;
                sendto(*socket_fd,buffer,strlen(buffer)+1,0,(sockaddr*)sa,sa_len);
        }
}

void Recv(int* socket_fd)
{
        char buffer[BUFFER_SIZE];
        while(!Exit)
        {
                int check=recvfrom(*socket_fd,buffer,sizeof(buffer),MSG_DONTWAIT,NULL,NULL);
                if(check>0)
                        cout<<"Server:"<<buffer<<endl;
        }
}

void test()
{
        int socket_fd=socket(AF_INET,SOCK_DGRAM,0);
        if(socket_fd==-1){perror("create socket");}

        struct sockaddr_in server_sa;
        server_sa.sin_family=PF_INET;
        server_sa.sin_port=htons(4399);
        inet_aton("192.168.232.135",&(server_sa.sin_addr));

        thread send_thread(Send,&socket_fd,&server_sa,sizeof(server_sa));
        thread recv_thread(Recv,&socket_fd);

        send_thread.join();
        recv_thread.join();
        close(socket_fd);
}

int main()
{
        test();

        cout<<"Program Exit!"<<endl;
        return 0;
}