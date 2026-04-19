#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<fstream>
#include<thread>
using namespace std;

void recvfile()
{
        int socket_fd=socket(AF_INET,SOCK_STREAM,0);
        if(socket_fd==-1){ perror("create socket");return;}

        sockaddr_in my_sa;
        my_sa.sin_family=PF_INET;
        my_sa.sin_port=htons(4399);
        my_sa.sin_addr.s_addr=htonl(INADDR_ANY);

        bind(socket_fd,(sockaddr*)&my_sa,sizeof(my_sa));
        listen(socket_fd,5);

        int new_fd=accept(socket_fd,NULL,NULL);
        if(new_fd==-1){perror("accept");return;}

        char buffername[64];
        int filesize=0;
        recv(new_fd,buffername,sizeof(buffername),0);
        recv(new_fd,&filesize,sizeof(filesize),0);

        char filename[64]="./tmp/";
        
        strcat(filename,buffername);
        cout<<"name:"<<filename<<" size:"<<filesize<<endl;

        fstream fo(filename,ios::out|ios::binary);
        if(!fo.is_open()){perror("fo.open");return;}

        char recvbuffer[64]="received";
        send(new_fd,recvbuffer,strlen(recvbuffer)+1,0);

        int write_once=12;
        char write_buffer[write_once];
        int recv_total=0;
        int recv_once=0;
        
		while(true)
        {
                recv_once=recv(new_fd,write_buffer,write_once,0);

                if(recv_once<write_once)
                        fo.write(write_buffer,strlen(write_buffer));
                else
                        fo.write(write_buffer,write_once);

                recv_total+=recv_once;
                if(recv_total>=filesize) break;
        }
        send(new_fd,recvbuffer,strlen(recvbuffer)+1,0);

        close(new_fd);
        close(socket_fd);
}

int main()
{
        recvfile();
        return 0;
}