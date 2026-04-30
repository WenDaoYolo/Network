...
//在原来封装的TcpServer类基础上添加新成员函数
void TcpServer::select_model()
{
    fd_set sfd;
    FD_ZERO(&sfd);
	
	FD_SET(listen_socket,&sfd);
	int max_fd=listen_socket;
    int no_evet_count=0;
    
	while(no_evet_count<=10)                                      //循环处理客户机请求，在连续10次的无效监听后服务器停止
	{
		fd_set readsfd=sfd;
	    struct timeval timeout;timeout.tv_sec=2;
	    int adjust=select(max_fd+1,&readsfd,NULL,NULL,&timeout);  //监听读事件
	    if(adjust<0){perror("call select");return;}               //<0调用失败
		
	    if(adjust==0)                                             //=0无事件
	    {
            std::cout<<"no event..."<<std::endl;
            no_evet_count++;
        }
	    else                                                      //>0有事件
	    {
	        for(int i=0;i<=max_fd;i++)                            //遍历位图判断事件
	        {
                no_evet_count=0;
	            if(!FD_ISSET(i,&readsfd))                         //跳过无读事件的socket
	                continue;

	            if(i==listen_socket)                              //如果是监听socket(连接队列的读取)
	            {
	                int new_fd=accept(listen_socket,NULL,NULL);
	                if(new_fd<0){perror("accept");};                
	                max_fd=new_fd>max_fd?new_fd:max_fd;           //更新最大监控值
                    FD_SET(new_fd,&sfd);                          //新socket加入位图
	                std::cout<<"cilent "<<new_fd<<" is join"<<std::endl;
	            }                                                 
	            else                                              //如果是通信socket(接收缓冲区的读取)
	            {
	                char recv_buffer[64]={0};
	                int message_len;
	                int check;
	                check=recv(i,&message_len,sizeof(message_len),0);
	                check=recv(i,recv_buffer,message_len,0);

	                if(check<=0)                                  //错误或者对方关闭连接
	                {
	                    std::cout<<"client "<<i<<" disconnect"<<std::endl;
	                    close(i);
	                    FD_CLR(i,&sfd);
 
	                    if(i==max_fd)                             //重新计算最大值
	                    {
	                        for(int j=max_fd;j>=0;j--)            //位图中有事件的socket可能不连续
	                        {
	                            if(FD_ISSET(j,&readsfd))
	                            {
	                                max_fd=j;
	                                break;
	                            }
	                        }
	                    }
	                    continue;
	                }
                    
                    //成功接收对方发送的报文
	                std::cout<<"client "<<i<<":"<<recv_buffer<<std::endl;
	                char send_message[]="received";
	                int send_len=strlen(send_message)+1;
	                send(i,&send_len,sizeof(send_len),0);
	                send(i,send_message,send_len,0);
	            }
	        }
	    }
    }
    
    for(int i=0;i<=max_fd;i++)                                  //防御性编程，防止漏关闭的socket
    {                                                           //不是为了解决已知的问题，而是为了不留未知的问题
	    if(FD_ISSET(i,&sfd))
		{
			close(i);
			std::cout<<"socket "<<i<<" is destory!"<<std::endl;
		}
    }
}