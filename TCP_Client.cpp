#include<iostream>
#include<thread>
#include<mutex>
#include<cstring>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#define BUFFER_SIZE 64

using namespace std;

bool Exit=false;
mutex CheckExit;

void Send(int* socket_fd)
{
        char buffer[BUFFER_SIZE]={0};
        while(true)
        {
                cin>>buffer;
                
                CheckExit.lock();
                if(!strcmp(buffer,"exit")) Exit=true;
                CheckExit.unlock();
                if(Exit) return;

                cout<<"[You]>";
                send(*socket_fd,buffer,strlen(buffer)+1,0);
        }
}

void Recv(int* socket_fd)
{
        char buffer[BUFFER_SIZE]={0};
        while(true)
        {
                CheckExit.lock();
                if(Exit){CheckExit.unlock();return;}
                CheckExit.unlock();

                recv(*socket_fd,buffer,sizeof(buffer),0);
                cout<<"[Server]>"<<(char*)buffer<<endl;
        }
}

void Disconnect(int* socket_fd)
{
        close(*socket_fd);
        cout<<"Disconnect Successfuly!"<<endl;
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
        Disconnect(&socket_fd);
}

int main()
{
        Connect();

        cout<<"Program Exit!"<<endl;
        return 0;
}