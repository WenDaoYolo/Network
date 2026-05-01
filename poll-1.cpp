...
//在原来封装的TcpServer类基础上添加新成员函数
void TcpServer::poll_model()
{
    pollfd polls[1024];
    for(int i=0;i<1024;i++)
        polls[i].fd=-1;

    polls[listen_socket].fd=listen_socket;
    polls[listen_socket].events=POLLIN;
    int maxfd=listen_socket;
    char sendbuffer[]="received";
    char recvbuffer[64];

    while(true)
    {
        int adjust=poll(polls,maxfd+1,-1);
        if(adjust<0){perror("call poll");continue;}                          //调用poll错误
        if(adjust==0){std::cout<<"no event..."<<std::endl;continue;}         //无事件跳过
        
        for(int i=0;i<=maxfd;i++)                                            //有事件，循环遍历数组
        {
            if(polls[i].fd<0) continue;                                      //错误跳过
            if((polls[i].revents)==0) continue;                              //无事件跳过

            if(polls[i].fd==listen_socket)                                   //已连接队列的读事件
            {
                int new_fd=accept(listen_socket,NULL,NULL);
                polls[new_fd].fd=new_fd;
                polls[new_fd].events=POLLIN;
                maxfd=new_fd>maxfd?new_fd:maxfd;                             //更新最大有效socket
                std::cout<<"client "<<new_fd<<" is join"<<std::endl;
            }
            else                                                             //缓冲区中有数据
            {
                int check=recv(polls[i].fd,recvbuffer,64,0);
                if(check<=0)                                                 //读取错误或对方断开连接
                {
                    std::cout<<"client "<<i<<" disconnect"<<std::endl;
                    polls[i].fd=-1;
                    polls[i].revents=0;
                    close(i);

                    if(i==maxfd)                                             //重新计算最大有效socket
                    {
                        for(int j=maxfd;j>=0;j--)
                        {
                            if(polls[j].fd!=-1)
                            {
                                maxfd=j;
                                break;
                            }
                        }
                    }
                    continue;
                }
                else
                {
                    std::cout<<"client "<<i<<":"<<recvbuffer<<std::endl;
                    send(polls[i].fd,sendbuffer,strlen(sendbuffer)+1,0);
                }
            }
        }
    }

    for(int j=0;j<=maxfd;j++)
    {
        if(polls[j].fd!=-1)
        {
            close(j);
            polls[j].fd=-1;
        }
    }
}