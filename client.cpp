#include "TcpClient.h"

int main()
{
    TcpClient tcpc1;
    if(tcpc1.Connect("192.168.232.135",4399)==-1)
        return 0;

    char send[]="hello,world!";
    tcpc1.SendMessage(send,strlen(send)+1);
    tcpc1.SendMessage(send,strlen(send)+1);

    sleep(3);
    char* recv_ptr=NULL;
    while((recv_ptr=tcpc1.RecvMessage())!=NULL)
    {
        std::cout<<"server:"<<recv_ptr<<std::endl;
        tcpc1.CloseMessage(recv_ptr);
    }

    tcpc1.SendFile("./","ttt1.txt",strlen("ttt1.txt")+1);
    tcpc1.RecvFile("./ttt2/");

    return 0;
}