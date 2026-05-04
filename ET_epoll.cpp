...
int setnonblocking(int fd)
{
    int flags;
    if((flags=fcntl(fd,F_GETFL))==-1)
        flags=0;
    return fcntl(fd,F_SETFL,flags|O_NONBLOCK);
}

void ET_epoll_model()
{
    signal(SIGPIPE,SIG_IGN);                                                     //忽略信号中断机制
    int listensocket=socket(AF_INET,SOCK_STREAM,0);
    setnonblocking(listensocket);                                                //设置非阻塞的socket
    sockaddr_in my_sa;
    my_sa.sin_family=PF_INET;
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
    my_sa.sin_port=htons(4399);
    bind(listensocket,(sockaddr*)&my_sa,sizeof(my_sa));
    listen(listensocket,5);
    
    int epolls=epoll_create(1);
    epoll_event listen_ev;
    listen_ev.events=EPOLLIN|EPOLLET;
    listen_ev.data.fd=listensocket;
    epoll_ctl(epolls,EPOLL_CTL_ADD,listensocket,&listen_ev);
    epoll_event recv_events[100];

    char recvbuffer[64];
    char sendbuffer[]="received";

    std::cout<<"listening.."<<std::endl;
    while(true)
    {
        int adjust=epoll_wait(epolls,recv_events,20,1000);
        if(adjust<0){perror("epoll_wait");continue;}
        if(adjust==0){std::cout<<"no event..."<<std::endl;continue;}

        for(int i=0;i<adjust;i++)                                                   //有事件，开始处理
        {
            
            if(recv_events[i].data.fd==listensocket)                                //已连接队列的读事件
            {
                int new_fd=accept(listensocket,NULL,NULL);
                if(new_fd==-1)                                                   
                {   
                    if(errno!=EAGAIN)                                               //accept错误
                    {
                        perror("accept");
                        continue;
                    }
                }
                else                                                                //accept成功
                {
                    epoll_event new_event;
                    new_event.events=EPOLLIN|EPOLLET;
                    new_event.data.fd=new_fd;
                    epoll_ctl(epolls,EPOLL_CTL_ADD,new_fd,&new_event);
                    std::cout<<"client "<<new_fd<<" is join"<<std::endl;
                    continue;
                }
            }
            
            if((recv_events[i].events)&POLLIN==1)                                  //缓冲区有可读数据
            {
                int cur_recv=0;
                memset(recvbuffer,0,64);
                char* position=recvbuffer;                                         //定义指针记录当前缓冲区的写入位置
                while(true)
                {
                    cur_recv=recv(recv_events[i].data.fd,position,5,MSG_DONTWAIT);
                    
                    if(cur_recv<=0)                                                //缓冲区无数据可读或对方断开连接
                    {
                        if(errno==EAGAIN)                                          //正常读完数据，不是错误
                        {
                            std::cout<<"client "<<recv_events[i].data.fd<<":"<<recvbuffer<<std::endl;
                            send(recv_events[i].data.fd,sendbuffer,strlen(sendbuffer)+1,0);
                        }
                        else                                                       //错误或对方断开连接
                        {
                            epoll_ctl(epolls,EPOLL_CTL_DEL,recv_events[i].data.fd,NULL);
                            close(recv_events[i].data.fd);
                            std::cout<<"client "<<recv_events[i].data.fd<<" disconnect"<<std::endl;
                        }
                        break;    
                    }
                    else                                                          //缓冲区有数据，继续读
                    {
                        position+=cur_recv;
                    }
                }
            }
        }
    }
}

int main()
{
    ET_epoll_model();

    return 0;
}