#include "InetAddress.h"

InetAddress::InetAddress(const std::string ip,uint16_t port)
{
    sai.sin_family=AF_INET;
    sai.sin_port=htons(port);
    inet_aton(ip.c_str(),&(sai.sin_addr));
}

InetAddress::InetAddress(){}

InetAddress::InetAddress(const sockaddr_in& other):sai(other){}

InetAddress::~InetAddress(){}

const sockaddr* InetAddress::addr(){ return (sockaddr*)&sai; }

const std::string InetAddress::ip(){ return inet_ntoa(sai.sin_addr); }

void InetAddress::SetAddrIn(sockaddr_in other){ sai=other; }

uint16_t InetAddress::port(){ return ntohs(sai.sin_port); }