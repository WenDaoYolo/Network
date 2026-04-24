#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<cstring>
#include<thread>

#define BUFFER_SIZE 64
using namespace std;

void Send(int* socket_fd)
{
        char buffer[BUFFER_SIZE]="hello,world!";
	int i=0;
        while(true)
        {
                send(*socket_fd,buffer,strlen(buffer)+1,0);
		i++;
		cout<<i<<": "<<buffer<<endl;
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

        send_thread.join();
        close(socket_fd);
}

int main()
{
        Connect();

        cout<<"Program Exit!"<<endl;
        return 0;
}
