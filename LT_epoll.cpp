...
void LT_epoll_model()
{
    int listensocket=socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in my_sa;
    my_sa.sin_family=PF_INET;
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
    my_sa.sin_port=htons(4399);
    bind(listensocket,(sockaddr*)&my_sa,sizeof(my_sa));
    listen(listensocket,5);
    
    int epolls=epoll_create(1);
    epoll_event listen_ev;
    listen_ev.events=EPOLLIN;
    listen_ev.data.fd=listensocket;
    epoll_ctl(epolls,EPOLL_CTL_ADD,listensocket,&listen_ev);
    epoll_event recv_events[100];

    char recvbuffer[64];
    char sendbuffer[]="received";
    bool firstc=true;

    std::cout<<"listening.."<<std::endl;
    while(true)
    {
        int adjust=epoll_wait(epolls,recv_events,20,2000);
        if(adjust<0){perror("epoll_wait");continue;}
        if(adjust==0){std::cout<<"no event..."<<std::endl;continue;}

        for(int i=0;i<adjust;i++)
        {
            if(recv_events[i].data.fd==listensocket)
            {
                if(firstc)        
                {
                    std::cout<<"fisrt listen event continue"<<std::endl;
                    firstc=false;
                    continue;                                                  //第一次跳过已连接队列事件，未处理下次继续
                }  

                int new_fd=accept(listensocket,NULL,NULL);
                epoll_event new_event;
                new_event.events=EPOLLIN;
                new_event.data.fd=new_fd;
                epoll_ctl(epolls,EPOLL_CTL_ADD,new_fd,&new_event);
                std::cout<<"client "<<new_fd<<" is join"<<std::endl;
            }
            else
            {
                if((recv_events[i].events)&POLLIN==1)                         //如果是写事件
                {
	                int check=recv(recv_events[i].data.fd,recvbuffer,5,0);    //只读取5个字节，未处理完的下次继续
	                if(check<=0)
	                {
	                    epoll_ctl(epolls,EPOLL_CTL_DEL,recv_events[i].data.fd,NULL);
	                    close(recv_events[i].data.fd);
	                    std::cout<<"client "<<recv_events[i].data.fd<<" disconnect"<<std::endl;
	                }
	                else
	                {
	                    std::cout<<"client "<<recv_events[i].data.fd<<":"<<recvbuffer<<std::endl;
	                    send(recv_events[i].data.fd,sendbuffer,strlen(sendbuffer)+1,0);
	                }
                }
            }
        }
    }
}

int main()
{
    LT_epoll_model();

    return 0;
}