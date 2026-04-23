#include "TcpServer.h"

int main()
{
    TcpServer tcps1(5);
    sockaddr_in client_sa;

    std::cout<<"listening..."<<std::endl;
    int new_fd=tcps1.Wait(client_sa);
    if(new_fd==-1) return 0;
    std::cout<<inet_ntoa(client_sa.sin_addr)<<" is join"<<std::endl;

    char send[]="I-am-Server!";
    tcps1.SendMessage(new_fd,send,strlen(send)+1);
    tcps1.SendMessage(new_fd,send,strlen(send)+1);

    sleep(3);
    char* recv_ptr=NULL;
    while((recv_ptr=tcps1.RecvMessage(new_fd))!=NULL)
    {
        std::cout<<"server:"<<recv_ptr<<std::endl;
        tcps1.CloseMessage(recv_ptr);
    }

    tcps1.RecvFile(new_fd,"./ttt/");
    tcps1.SendFile(new_fd,"./","ttt2.txt",strlen("ttt2.txt")+1);

    close(new_fd);
    return 0;
}