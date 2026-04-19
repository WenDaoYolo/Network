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

void RW(int* socket_fd)
{
        char buffer[BUFFER_SIZE]={0};
        char Message[]="Received!";
        sockaddr_in client_sa;
        socklen_t client_len=sizeof(client_sa);
        while(!Exit)
        {
                int check=recvfrom(*socket_fd,buffer,sizeof(buffer),MSG_DONTWAIT,(sockaddr*)&client_sa,&client_len);

                if(check>0)
                {
                        cout<<"Client:"<<buffer<<endl;
			            sendto(*socket_fd,Message,strlen(Message)+1,0,(sockaddr*)&client_sa,sizeof(client_sa)); 
			            if(!strcmp(buffer,"exit")) Exit=true;      
                }
        }
}

void Wait()
{
        int socket_fd=socket(AF_INET,SOCK_DGRAM,0);
        if(socket_fd==-1){perror("create socket");return;}

        struct sockaddr_in my_sa;
        my_sa.sin_family=PF_INET;
        my_sa.sin_port=htons(atoi("4399"));
        my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
        bind(socket_fd,(sockaddr*)&my_sa,sizeof(my_sa));

        thread rw_thread(RW,&socket_fd);
        rw_thread.join();
}

int main()
{
        Wait();

        cout<<"Program Exit!"<<endl;
        return 0;
}