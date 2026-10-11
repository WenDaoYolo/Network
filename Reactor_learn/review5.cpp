#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<iostream>
#include<unistd.h>
#include<memory.h>

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

bool Read(int& client_fd)
{
    char buffer[64];
    while(memset(buffer,0,sizeof(buffer)),true)
    {
        int read_nums=read(client_fd,buffer,sizeof(buffer));
        if(read_nums==-1)
            perror("read:");

        if(read_nums==0)
        {
            std::cout<<"client"<<client_fd<<" diconnect"<<std::endl;
            break;
        }

        std::cout<<"current read nums:"<<read_nums<<std::endl;
        std::cout<<"client"<<client_fd<<":"<<buffer<<std::endl;
    }
    return true;
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
        
        if(Read(client_fd))
        {
            close(client_fd);
            std::cout<<"server will be down"<<std::endl;
            sleep(2);
            break;
        }
    }
}

int main()
{
    int listen_fd;
    InitSocket(listen_fd);
    Loop(listen_fd);

    return 0;
}