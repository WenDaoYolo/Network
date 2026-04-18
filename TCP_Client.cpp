#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>
#include<mutex>

#define BUFFER_SIZE 64
using namespace std;
bool Exit=false;
mutex RWExit;

void Send(int* socket_fd)
{
        char buffer[BUFFER_SIZE];
        while(!Exit)
        {
                cin>>buffer;

                if(!strcmp(buffer,"exit"))
                {
                        lock_guard<mutex> lg_mtx(RWExit);
                        Exit=true;
                }
                send(*socket_fd,buffer,strlen(buffer)+1,0);
        }
}

void Recv(int* socket_fd)
{
        char buffer[BUFFER_SIZE];
        while(!Exit)
        {
                int check=recv(*socket_fd,buffer,sizeof(buffer),MSG_DONTWAIT);
                if(check>0)
                        cout<<"Server:"<<buffer<<endl;
                if(check==0)
                {
                        lock_guard<mutex> lg_mtx(RWExit);
                        cout<<"Server is Disconnect,Press anykey + 'Enter' to exit"<<endl;
                        Exit=true;
                }
        }
}

void Connect()
{
        int socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if(socket_fd==-1){perror("create socket");}

        struct sockaddr_in server_sa;
        server_sa.sin_family=PF_INET;
        server_sa.sin_port=htons(atoi("4399"));
        inet_aton("192.168.232.135",&(server_sa.sin_addr));

        int check=connect(socket_fd,(sockaddr*)&server_sa,sizeof(server_sa));
        if(check==-1){perror("connect");return;}

        thread send_thread(Send,&socket_fd);
        thread recv_thread(Recv,&socket_fd);

        send_thread.join();
        recv_thread.join();
        close(socket_fd);
}

int main()
{
        Connect();

        cout<<"Program Exit!"<<endl;
        return 0;
}