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

class TcpClient
{
    private:
        int my_socket;
        struct sockaddr_in target_sa;
    public:
        TcpClient();
        ~TcpClient();

        //return dynamic message
        char* RecvMessage();

        //close dynamic message                      
        void CloseMessage(char* message);  
        
        //storage directory
        void RecvFile(const char* directory);         

        //file directory,filename,filename len(include'\0');
        void SendFile(const char* dir,const char* filename,int filename_len);
        bool Connect(const char* ip,short port);

        //real len(include'\0')
        void SendMessage(const char* m,int m_len);
};