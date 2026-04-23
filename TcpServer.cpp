#include "TcpServer.h"

TcpServer::TcpServer(int queue_len)
{
    listen_socket=socket(AF_INET,SOCK_STREAM,0);
    if(listen_socket==-1){perror("create socket");}

    my_sa.sin_family=PF_INET;
    my_sa.sin_port=htons(4399);
    my_sa.sin_addr.s_addr=htonl(INADDR_ANY);
    bind(listen_socket,(sockaddr*)&my_sa,sizeof(my_sa));
    listen(listen_socket,queue_len);
}

int TcpServer::Wait(sockaddr_in& client_sa)
{
    socklen_t c_len=sizeof(client_sa);
    int new_fd=accept(listen_socket,(sockaddr*)&client_sa,&c_len);
    if(new_fd==-1){perror("accept");return -1;}
    return new_fd;
}

void TcpServer::SendMessage(int client_fd,const char* m,int m_len)
{
    send(client_fd,&m_len,sizeof(m_len),0);
    send(client_fd,m,m_len,0);
}

char* TcpServer::RecvMessage(int client_fd)
{
    int m_len;
    if(recv(client_fd,&m_len,sizeof(m_len),MSG_DONTWAIT)<=0)
        return NULL;

    char* m_ptr=new char[m_len];

    if(recv(client_fd,m_ptr,m_len,MSG_DONTWAIT)<=0)
    {
        delete m_ptr;
        return NULL;
    }
    return m_ptr;
}

void TcpServer::CloseMessage(char* m){delete m;}

TcpServer::~TcpServer(){close(listen_socket);}

void TcpServer::SendFile(int client_fd,const char* dir,const char* filename,int filename_len)
{
    char total_filename[64];
    strcpy(total_filename,dir);
    strcat(total_filename,filename);
    std::fstream f1(total_filename,std::ios::in|std::ios::binary);
    if(!f1.is_open()){perror("open file");return;}
    int filesize=(f1.seekg(0,std::ios::end)).tellg();
    f1.seekg(0,std::ios::beg);//remember to reset the get pointer!
    
    send(client_fd,&filename_len,sizeof(filename_len),0);
    send(client_fd,filename,filename_len,0);
    send(client_fd,&filesize,sizeof(filesize),0);

    char check[32]={0};
    recv(client_fd,check,sizeof(check),0);
    if(strcmp(check,"received"))
    {
        std::cout<<"lost!"<<std::endl;
        return;
    }

    char read_once[64]={0};
    while(f1.read(read_once,sizeof(read_once))||f1.gcount()>0)
        send(client_fd,read_once,f1.gcount(),0);
    
    recv(client_fd,check,sizeof(check),0);
    if(strcmp(check,"received"))
    {
        std::cout<<"lost!"<<std::endl;
        return;
    }
    std::cout<<"send success! file name:"<<filename<<" file size:"<<filesize<<std::endl;
}

void TcpServer::RecvFile(int client_fd,const char* directory)
{
    char file_directory[64];
    strcpy(file_directory,directory);
    int filename_len=-1,file_size=0;
    recv(client_fd,&filename_len,sizeof(filename_len),0);
    char* ptr= new char[filename_len];
    recv(client_fd,ptr,filename_len,0);
    recv(client_fd,&file_size,sizeof(file_size),0);
    strcat(file_directory,ptr);
    delete ptr;
    
    std::fstream f1(file_directory,std::ios::out|std::ios::binary);
    if(!f1.is_open()){perror("open file");return;}
    send(client_fd,"received",sizeof("received"),0);

    int recv_count=0,recv_cur=0;
    char recv_once[64]={0};
    while(recv_count<file_size)
    {
        recv_cur=0;
        recv_cur=recv(client_fd,recv_once,sizeof(recv_once),MSG_DONTWAIT);
        if(recv_cur>0)
        {
            f1.write(recv_once,recv_cur);
            recv_count+=recv_cur;
        }
    }
    send(client_fd,"received",sizeof("received"),0);
    std::cout<<"file directory:"<<file_directory<<" file size:"<<file_size<<std::endl;
}