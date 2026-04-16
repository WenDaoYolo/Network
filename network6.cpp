void connect_to()
{
        int client_fd=socket(AF_INET,SOCK_STREAM,0);
        sockaddr_in server_sai;
        server_sai.sin_family=AF_INET;
        server_sai.sin_port=htons(atoi("4399"));
        hostent* h=gethostbyname("192.168.232.135");
        memcpy(&(server_sai.sin_addr.s_addr),h->h_addr_list[0],h->h_length);

        int check=connect(client_fd,(sockaddr*)(&server_sai),sizeof(server_sai));
        if(check==-1){perror("connect server");return;}
        cout<<"connect success!"<<endl;
        cout<<"server address:"<<inet_ntoa(server_sai.sin_addr)<<endl;

        close(client_fd);
}