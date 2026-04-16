void test1()
{
        int client_fd=socket(AF_INET,SOCK_STREAM,0);
        sockaddr_in server_sai;
        server_sai.sin_family=AF_INET;
        server_sai.sin_port=htons(atoi("21"));
        hostent* h=gethostbyname("www.baidu.com");
        memcpy(&(server_sai.sin_addr.s_addr),h->h_addr_list[0],h->h_length);

        int check=connect(client_fd,(sockaddr*)(&server_sai),sizeof(server_sai));
        if(check==-1){perror("connect to baidu");return;}
        cout<<"connect success!"<<endl;
        cout<<"server address:"<<inet_ntoa(server_sai.sin_addr)<<endl;
	    close(client_fd);
}

int main()
{
	test1();
	return 0;
}