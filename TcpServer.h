#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<iostream>
#include<cstring>
#include<fstream>
#include<thread>
#include<mutex>
#pragma once

class TcpServer{
    private:
        int listen_socket;
        struct sockaddr_in my_sa;
    public:
        TcpServer(int queue_len);
        ~TcpServer();

        int Wait(sockaddr_in& client_sa);

        //return dynamic message
        char* RecvMessage(int client_fd);

        //close dynamic message          
        void CloseMessage(char* m);
        
        //storage directory
        void RecvFile(int client_fd,const char* directory);
        
        //file directory,filename,filename len(include'\0');
        void SendFile(int client_fd,const char* dir,const char* filename,int filename_len);

        //real len(include'\0')
        void SendMessage(int client_fd,const char* m,int m_len);
};