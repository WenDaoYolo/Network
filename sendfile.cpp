#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<fstream>
#include<thread>
#include<memory.h>
using namespace std;

void sendfile()
{
        int socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if(socket_fd==-1){ perror("create socket");return;}

        sockaddr_in server_sa;
        server_sa.sin_family=PF_INET;
        server_sa.sin_port=htons(4399);
        inet_aton("192.168.232.135",&(server_sa.sin_addr));

        int check=connect(socket_fd,(sockaddr*)&server_sa,sizeof(server_sa));
        if(check==-1){perror("connect");return;}

        char filename[64];int filesize=11;
        cout<<"filename:";cin>>filename;
        send(socket_fd,filename,strlen(filename)+1,0);
        cout<<"filesize:";cin>>filesize;
        send(socket_fd,&filesize,sizeof(filesize),0);

        char recvbuffer[64];
        recv(socket_fd,recvbuffer,sizeof(recvbuffer),0);

		if(!strcmp(recvbuffer,"received"))
        {
                cout<<"server:"<<recvbuffer<<endl;
                fstream f1(filename,ios::in|ios::binary);
                if(!f1.is_open()){perror("f1.open()");return;}

                int read_once=12;
                char read_buffer[read_once]={0};
                while(true)
                {
                        if(memset(read_buffer,0,read_once),f1.read(read_buffer,read_once))
                                send(socket_fd,read_buffer,read_once,0);
                        else
                        {
                                send(socket_fd,read_buffer,strlen(read_buffer)+1,0);
                                break;
                        }
                }
        }
        else
        {
                cout<<"lost!"<<endl;
                return;
        }

        recv(socket_fd,recvbuffer,sizeof(recvbuffer),0);

        if(!strcmp(recvbuffer,"received"))
                cout<<"server:"<<recvbuffer<<endl;
        else
        {
                cout<<"lost!"<<endl;
                return;
        }
        close(socket_fd);
}

int main()
{
        sendfile();
        return 0;
}