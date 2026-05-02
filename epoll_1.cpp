...
//在原来封装的TcpServer类基础上添加新成员函数
void TcpServer::epoll_model()
{
    epoll_event events[20];
    epoll_event listen_event;
    int epolls=epoll_create(1);
    listen_event.events=POLLIN;
    listen_event.data.fd=listen_socket;
    epoll_ctl(epolls,EPOLL_CTL_ADD,listen_socket,&listen_event);
    char recvbuffer[64];
    char sendbuffer[]="received";

    while(true)
    {
        int adjust=epoll_wait(epolls,events,20,1000);

        if(adjust<0){perror("call epoll_wait");continue;}
        if(adjust==0){std::cout<<"no event..."<<std::endl;continue;}

        for(int i=0;i<adjust;i++)
        {
            if(events[i].data.fd==listen_socket)
            {
                int new_fd=accept(listen_socket,NULL,NULL);
                if(new_fd<0){perror("accept");continue;}
                epoll_event client_event;
                client_event.events=POLLIN;
                client_event.data.fd=new_fd;
                epoll_ctl(epolls,EPOLL_CTL_ADD,new_fd,&client_event);
                std::cout<<"client "<<new_fd<<" is join"<<std::endl;
            }
            else
            {
                if(recv(events[i].data.fd,recvbuffer,64,0)<=0)
                {
                    std::cout<<"client "<<events[i].data.fd<<" disconnect"<<std::endl;
                    epoll_ctl(epolls,EPOLL_CTL_DEL,events[i].data.fd,NULL);
                    close(events[i].data.fd);
                }
                else
                {
                    std::cout<<"client "<<events[i].data.fd<<":"<<recvbuffer<<std::endl;
                    send(events[i].data.fd,sendbuffer,strlen(sendbuffer)+1,0);
                }
            }
        }
    }
}