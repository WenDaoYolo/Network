#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<unistd.h>
#include<iostream>
#include<cstring>
#include<fcntl.h>

int setnoblocking(int fd)
{
    int flags;
    if((flags=fcntl(fd,F_GETFL))==-1)
        flags=0;
    return fcntl(fd,F_SETFL,O_NONBLOCK); 
    //or flags|O_NONBLOCK->avoid is blocking?
}

int main()
{
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    setnoblocking(server_fd);

    sockaddr_in server_sa;
    server_sa.sin_family=PF_INET;
    server_sa.sin_port=htons(4399);
    inet_aton("192.168.232.135",&(server_sa.sin_addr));
    std::cout<<"target:192.168.232.135"<<std::endl;

    if(connect(server_fd,(sockaddr*)&server_sa,sizeof(server_sa))==-1)
    {
        pollfd check;
        check.events=POLLOUT;
        check.fd=server_fd;
        check.revents=-1;
        poll(&check,check.fd,0);

        if(errno!=EINPROGRESS||(check.revents&POLLOUT)==0)
        {
            perror("conncet");
            return -1;
        }   
        else
        {
            std::cout<<"connect success!"<<std::endl;
        }
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/poll.h>
#include<unistd.h>
#include<iostream>
#include<cstring>
#include<fcntl.h>

int setnoblocking(int fd)
{
    int flags;
    if((flags=fcntl(fd,F_GETFL,0))==-1)
        flags=0;
    return fcntl(fd,F_SETFL,flags|O_NONBLOCK);
}

int main()
{
    int server_fd=socket(AF_INET,SOCK_STREAM,0);
    setnoblocking(server_fd);

    sockaddr_in server_sa;
    server_sa.sin_family=PF_INET;
    server_sa.sin_port=htons(4399);
    inet_aton("12.18.22.15",&(server_sa.sin_addr));
    std::cout<<"target:12.18.22.15"<<std::endl;

    if(connect(server_fd,(sockaddr*)&server_sa,sizeof(server_sa))==-1)
    {
        pollfd check;
        check.events=POLLOUT;
        check.fd=server_fd;
        check.revents=-1;
        poll(&check,check.fd,0);

        if(errno!=EINPROGRESS||(check.revents&POLLOUT)==0)
        {
            perror("conncet");
            return -1;
        }   
        else
        {
            std::cout<<"connect success!"<<std::endl;
        }
    }

    return 0;
}
