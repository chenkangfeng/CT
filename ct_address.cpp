//
//  ct_address.cpp 2016-6-22
//

#include "ct_address.h"

using namespace std;

ct_namespace_begin

addr_info::addr_info(void)
{
    memset(&info_, 0, sizeof(info_));
    memset(ip_, 0, sizeof(ip_));
}

addr_info::addr_info(sockaddr* addr, socklen_t len)
{
    memset(&info_, 0, sizeof(info_));
    memset(ip_, 0, sizeof(ip_));
    
    memcpy(&info_, addr, len);
    if(family() == AF_INET){
        evutil_inet_ntop(AF_INET, &info_.ipv4.sin_addr, ip_, sizeof(ip_));
    }else if(family() == AF_INET6){
        evutil_inet_ntop(AF_INET6, &info_.ipv6.sin6_addr, ip_, sizeof(ip_));
    }
}

addr_info::~addr_info(void)
{
}

const sockaddr& addr_info::addr(void) const
{
    return info_.generic;
}

socklen_t addr_info::addr_len(void) const
{
    socklen_t ret = 0;
    if(family() == AF_INET){
        ret = sizeof(info_.ipv4);
    }else if(family() == AF_INET6){
        ret = sizeof(info_.ipv6);
    }
    return ret;
}

sa_family_t addr_info::family(void) const
{
    return info_.generic.sa_family;
}

ct_strptr addr_info::ip(void) const
{
    return ip_;
}

void addr_info::set_port(in_port_t port)
{
    if(family() == AF_INET){
        info_.ipv4.sin_port = htons(port);
    }else if(family() == AF_INET6){
        info_.ipv6.sin6_port = htons(port);
    }
}

address::address(void)
{
}

address::~address(void)
{
}

void address::init(evutil_addrinfo* addr)
{
    while(addr != NULL){
        addr_vec_.push_back(addr_info(addr->ai_addr, addr->ai_addrlen));
        addr = addr->ai_next;
    }
}

ct_int32 address::size(void)
{
    return (ct_int32)addr_vec_.size();
}

addr_info& address::operator[](ct_int32 index)
{
    return addr_vec_[index];
}

ct_namespace_end
