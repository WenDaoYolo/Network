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
   
void Recv(int* socket_fd)
{
        char buffer[BUFFER_SIZE];
        while(true)
        {
		usleep(20);
                int check=recv(*socket_fd,buffer,sizeof(buffer),MSG_DONTWAIT);
                if(check>0)
                {
                        cout<<"Client:"<<buffer<<endl;
                }
        }
}

void Wait()
{
        int listen_fd=socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd==-1){perror("create socket");return;}

        struct sockaddr_in my_sa;
        my_sa.sin_family=PF_INET;
        my_sa.sin_port=htons(atoi("4399"));
        my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
        bind(listen_fd,(sockaddr*)&my_sa,sizeof(my_sa));

        listen(listen_fd,5);

        sockaddr_in client_sa;
        socklen_t client_len=sizeof(client_sa);
        cout<<"listening..."<<endl;
        int new_fd=accept(listen_fd,(sockaddr*)&client_sa,&client_len);

        if(new_fd==-1){perror("accpet");return;}
        cout<<inet_ntoa(client_sa.sin_addr)<<" is join"<<endl;

	thread recv_thread(Recv,&new_fd);
        recv_thread.join();

        close(new_fd);
        close(listen_fd);
}

int main()
{
        Wait();

        cout<<"Program Exit!"<<endl;
        return 0;
}
