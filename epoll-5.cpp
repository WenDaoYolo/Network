...
int setnonblocking(int fd)
{
    int flags;
    if((flags=fcntl(fd,F_GETFL))==-1)
        flags=0;
    return fcntl(fd,F_SETFL,flags|O_NONBLOCK);
}

int main()
{
    int listen_socket=socket(PF_INET,SOCK_STREAM,0);
    setnonblocking(listen_socket);
    sockaddr_in my_sa;
    my_sa.sin_family=PF_INET;
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
    my_sa.sin_port=htons(4399);

    bind(listen_socket,(sockaddr*)&my_sa,sizeof(my_sa));    
    listen(listen_socket,5);
    bool IsProcess=false;

    std::cout<<"listening..."<<std::endl;
    while(true)
    {
        int fd=accept(listen_socket,NULL,NULL);

        if(fd==-1)
        {   
            if(errno!=EAGAIN)
            {
                perror("accept");
                return -1;
            }
        }
        else
        {
            std::cout<<"client "<<fd<<" is join"<<std::endl;
            IsProcess=true;
        }

        if(IsProcess)
        {
            //循环处理一轮事件...
        }
    }
    return 0;
}