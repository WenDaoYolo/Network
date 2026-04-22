#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>
#define BUFFER_SIZE 64

using namespace std;
int client_id=0;

void RW(int socket_fd,int id)
{
	char recv_buffer[BUFFER_SIZE]={0};
	char send_buffer[BUFFER_SIZE]="received!";
	while(true)
	{
		recv(socket_fd,recv_buffer,sizeof(recv_buffer),0);
		cout<<"client "<<id<<":"<<recv_buffer<<endl;
		
		send(socket_fd,send_buffer,strlen(send_buffer)+1,0);
		
		if(!strcmp(recv_buffer,"exit"))
		{
			cout<<"client "<<id<<" disconncet!"<<endl;
			close(socket_fd);
			return;
		}
	}
}

void Wait()
{
	int listen_fd=socket(AF_INET,SOCK_STREAM,0);
	if(listen_fd==-1){perror("create socket");return;}
	
	sockaddr_in my_sa;
	my_sa.sin_family=PF_INET;
	my_sa.sin_port=htons(4399);
	my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
	bind(listen_fd,(sockaddr*)&my_sa,sizeof(my_sa));
	listen(listen_fd,2);
	
	while(true)
	{
		cout<<"listening..."<<endl;
		int new_fd=accept(listen_fd,NULL,NULL);	
	
		thread rw_thread(RW,new_fd,client_id);
		cout<<"client "<<client_id<<" is join!"<<endl;
				
		client_id++;
		rw_thread.detach();
	}
}

int main()
{
	Wait();
	cout<<"Program Exit!"<<endl;
	return 0;
}
