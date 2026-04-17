#include<iostream>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<memory.h>

using namespace std;

void test2()
{
        int listen_fd=socket(AF_INET,SOCK_STREAM,0);
        if(listen_fd==-1){perror("create socket");return;}
        cout<<"create socket success!"<<endl;

        sockaddr_in listen_sai;
        listen_sai.sin_family=AF_INET;
        listen_sai.sin_port=htons(atoi("4399"));
        listen_sai.sin_addr.s_addr=htonl(INADDR_ANY);

        int check=bind(listen_fd,(sockaddr*)&listen_sai,sizeof(listen_sai));
        if(check==-1){perror("bind socket");return;}
        cout<<"bind socket success!"<<endl;

        check=listen(listen_fd,5);
        if(check==-1){perror("listen");return;}
        if(check==0) cout<<"listening..."<<endl;

	socklen_t addr_len=sizeof(listen_sai);
        int new_fd=accept(listen_fd,(sockaddr*)&listen_sai,&addr_len);
        if(new_fd==-1){perror("accept");return;}
        cout<<"accept success! start to transimission!"<<endl;

        char SendBuffer[64];
        char RecvBuffer[64];

        recv(new_fd,RecvBuffer,sizeof(RecvBuffer),0);
        cout<<"Client:"<<(char*)RecvBuffer<<endl;

        cout<<"me>";
        cin>>SendBuffer;
        send(new_fd,SendBuffer,sizeof(SendBuffer),0);

        close(new_fd);
        close(listen_fd);
}

int main()
{
        test2();

        return 0;
}

