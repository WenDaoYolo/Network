#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>

void InitSocket(int& listen_fd)
{
    listen_fd=socket(AF_INET,SOCK_STREAM,0);
    if(listen_fd==-1)
        throw std::runtime_error("create socket failed!");

    sockaddr_in sai;
    sai.sin_family=AF_INET;
    sai.sin_port=htons(2026);
    sai.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(listen_fd,(sockaddr*)&sai,sizeof(sai));
    if(listen(listen_fd,5)==-1)
        throw std::runtime_error("listen fd failed!");
}

void Loop(int& listen_fd)
{
    sockaddr_in tmp;
    socklen_t len_ptr=sizeof(tmp);
    while(true)
    {
        std::cout<<"listening..."<<std::endl;
        int client_fd=accept(listen_fd,(sockaddr*)&tmp,&len_ptr);
        if(client_fd==-1)
        {
            std::cout<<"accpet error!"<<std::endl;
            continue;
        }
        std::cout<<"client "<<client_fd<<" connect!"<<std::endl;
        std::cout<<"ip:"<<inet_ntoa(tmp.sin_addr)<<std::endl;
        close(client_fd);
    }
}

int main()
{
    int listen_fd;
    InitSocket(listen_fd);
    Loop(listen_fd);

    return 0;
}