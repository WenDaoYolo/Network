#include "TcpClient.h"

TcpClient::TcpClient()
{
    my_socket=socket(AF_INET,SOCK_STREAM,0);
    if(my_socket==-1)
        perror("create socket");
    target_sa.sin_family=PF_INET;
}

bool TcpClient::Connect(const char* ip,short port)
{
    inet_aton(ip,&(target_sa.sin_addr));
    target_sa.sin_port=htons(port);
    int check=connect(my_socket,(sockaddr*)&(target_sa),sizeof(target_sa));
    if(check==-1){ perror("connect");return false;}
    return true;
}

void TcpClient::SendMessage(const char* m,int m_len)
{
    send(my_socket,&m_len,sizeof(m_len),0);
    send(my_socket,m,m_len,0);
}

char* TcpClient::RecvMessage()
{
    int m_len;
    if(recv(my_socket,&m_len,sizeof(m_len),MSG_DONTWAIT)<=0)
        return NULL;

    char* m_ptr=new char[m_len];

    if(recv(my_socket,m_ptr,m_len,MSG_DONTWAIT)<=0)
    {
        delete m_ptr;
        return NULL;
    }
    return m_ptr;
}

void TcpClient::CloseMessage(char* message){delete message;}

TcpClient::~TcpClient(){close(my_socket);}

void TcpClient::SendFile(const char* dir,const char* filename,int filename_len)
{
    char total_filename[64];
    strcpy(total_filename,dir);
    strcat(total_filename,filename);
    std::fstream f1(total_filename,std::ios::in|std::ios::binary);
    if(!f1.is_open()){perror("open file");return;}
    int filesize=(f1.seekg(0,std::ios::end)).tellg();
    f1.seekg(0,std::ios::beg);//remember to reset the get pointer!
    
    send(my_socket,&filename_len,sizeof(filename_len),0);
    send(my_socket,filename,filename_len,0);
    send(my_socket,&filesize,sizeof(filesize),0);

    char check[32]={0};
    recv(my_socket,check,sizeof(check),0);
    if(strcmp(check,"received"))
    {
        std::cout<<"lost!"<<std::endl;
        return;
    }

    char read_once[64]={0};
    while(f1.read(read_once,sizeof(read_once))||f1.gcount()>0)
        send(my_socket,read_once,f1.gcount(),0);
    
    recv(my_socket,check,sizeof(check),0);
    if(strcmp(check,"received"))
    {
        std::cout<<"lost!"<<std::endl;
        return;
    }
    std::cout<<"send success! file name:"<<filename<<" file size:"<<filesize<<std::endl;
}

void TcpClient::RecvFile(const char* directory)
{
    char file_directory[64];
    strcpy(file_directory,directory);
    int filename_len=-1,file_size=0;
    recv(my_socket,&filename_len,sizeof(filename_len),0);
    char* ptr= new char[filename_len];
    recv(my_socket,ptr,filename_len,0);
    recv(my_socket,&file_size,sizeof(file_size),0);
    strcat(file_directory,ptr);
    delete ptr;
    
    std::fstream f1(file_directory,std::ios::out|std::ios::binary);
    if(!f1.is_open()){perror("open file");return;}
    send(my_socket,"received",sizeof("received"),0);

    int recv_count=0,recv_cur=0;
    char recv_once[64]={0};
    while(recv_count<file_size)
    {
        recv_cur=0;
        recv_cur=recv(my_socket,recv_once,sizeof(recv_once),MSG_DONTWAIT);
        if(recv_cur>0)
        {
            f1.write(recv_once,recv_cur);
            recv_count+=recv_cur;
        }
    }
    send(my_socket,"received",sizeof("received"),0);
    std::cout<<"file directory:"<<file_directory<<" file size:"<<file_size<<std::endl;
}