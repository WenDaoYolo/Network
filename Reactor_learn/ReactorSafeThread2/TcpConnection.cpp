#include "TcpConnection.h"

TcpConnection::TcpConnection(int fd,Epoll* ep,CRPC_ThreadPool* tp,TaskQueue* taskq):ep_(ep),sock_(fd),ch_(fd,ep),tp_(tp),taskqueue_(taskq)
{    
    ch_.EnableReading();
    ch_.SetET();
}

void TcpConnection::Init()//sate bind callback
{
    auto self = shared_from_this();
    
    ch_.SetErrorCallBack([self]() { self->HandleClose(); });
    ch_.SetReadCallBack([self]() { self->HandleRead(); });
    ch_.SetWriteCallBack([self]() { self->HandleWrite(); });
    
    //只有绑定了回调，才能注册到 epoll，防止事件先于回调触发
    ep_->Update(&ch_);
}

void TcpConnection::SetCloseCallBack(std::function<void(int)> fun){ closecallback_=fun; }

TcpConnection::~TcpConnection(){}

void TcpConnection::HandleClose()
{
    if(ch_.inepoll())
        ep_->Del(&ch_);
    else 
        return;//如果不在红黑树则直接退出，防止重复调用关闭连接的回调
    
    if(closecallback_) closecallback_(sock_.fd());//关闭后该内存会销毁，不要再执行仍和访问操作
}

//////////////////////////////////////////////////
void TcpConnection::Register()
{
    //to do...
    
    auto ptr=shared_from_this();
    taskqueue_->push
    (
        [ptr]()
        {
        std::string response = "register success";
        int len = response.size();
        int net_len = htonl(len); // 转成网络字节序

        // --- 第一部分：写入 4 字节的长度头 ---
        // 1. 确保有 4 字节空间
        ptr->sendbuffer_.CheckWritAble(sizeof(int)); 
        // 2. 把长度头拷贝到写指针处
        memcpy(ptr->sendbuffer_.WritePtr(), &net_len, sizeof(int)); 
        // 3. 移动写指针
        ptr->sendbuffer_.MoveWritePtr(sizeof(int)); 

        // --- 第二部分：写入实际内容 ---
        // 1. 确保有足够空间
        ptr->sendbuffer_.CheckWritAble(len);
        // 2. 把字符串内容拷贝到写指针处
        memcpy(ptr->sendbuffer_.WritePtr(), response.c_str(), len);
        // 3. 移动写指针
        ptr->sendbuffer_.MoveWritePtr(len);

        ptr->HandleWrite();
        }
    );
}

void TcpConnection::Login()
{
    //to do...
}

void TcpConnection::Other1()
{
    //to do...
}

void TcpConnection::Other2()
{
    //to do...
}
//////////////////////////////////////////////////

void TcpConnection::HandleRead()
{
    while(true)
    {
        size_t readables=recvbuffer_.CheckWritAble(1024);
        int nums=recv(sock_.fd(),recvbuffer_.WritePtr(),readables,0);

        if(nums<0)
        {
            if(errno==EAGAIN) break;
            std::cout<<"call recv failed"<<std::endl;
            HandleClose();
            return;//同理内存会销毁，直接返回，不要再执行任何访问操作
        }
        else if(nums==0)
        {
            HandleClose();
            return;//同理内存会销毁，直接返回，不要再执行任何访问操作
        }
        else
        {
            recvbuffer_.MoveWritePtr(nums);
        }
    }

    while(recvbuffer_.ReadAbleBytes()>=4)
    {
        int len=0;
        memcpy(&len,recvbuffer_.ReadPtr(),sizeof(int));
        len=ntohl(len);
        
        if(recvbuffer_.ReadAbleBytes()>=4+len)
        {
            recvbuffer_.MoveReadPrt(4);
            std::string msg(recvbuffer_.ReadPtr(),len);
            recvbuffer_.MoveReadPrt(len);
            auto sptr=shared_from_this();

            if(!strcmp(msg.c_str(),"register"))
                tp_->enqueue([sptr](){sptr->Register();});
            if(!strcmp(msg.c_str(),"login"))
                tp_->enqueue([sptr](){sptr->Login();});
            if(!strcmp(msg.c_str(),"other1"))
                tp_->enqueue([sptr](){sptr->Other1();});
            if(!strcmp(msg.c_str(),"other2"))
                tp_->enqueue([sptr](){sptr->Other2();});
        }
        else
            break;//如果数据不完整则跳出循环，等待下次解析
    }
}

void TcpConnection::HandleWrite()
{
    // 如果缓冲区里没东西可发，直接返回
    if (sendbuffer_.ReadAbleBytes() == 0) {
        return;
    }

    // 如果当前没有在监听写事件（说明上一次没遇到 EAGAIN）
    if (!ch_.IsWriting()) {
        // 尝试把 SendBuffer 里的数据推给内核
        ssize_t n = send(sock_.fd(), sendbuffer_.ReadPtr(), sendbuffer_.ReadAbleBytes(), 0);
        
        if (n > 0) {
            // 成功发了 n 个字节，把读指针往后移
            sendbuffer_.MoveReadPrt(n);
        } else if (n < 0) {
            // n < 0 且不是 EAGAIN，说明对端断开了或发生错误
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                HandleClose(); // 直接走关闭逻辑
                return;
            }
        }
    }

    // 核心：如果 SendBuffer 里还有数据没发完（内核满了，返回了 EAGAIN）
    if (sendbuffer_.ReadAbleBytes() > 0) {
        // 注册 EPOLLOUT 写事件，告诉 epoll：“等内核有空间了，你叫我”
        ch_.EnableWriting(); 
        ep_->Update(&ch_);
    }
}