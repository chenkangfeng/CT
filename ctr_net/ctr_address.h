//
//  ctr_address.h 2016-07-04
//  网络地址
//

#ifndef _CTR_ADDRESS_H_
#define _CTR_ADDRESS_H_

#include "ctr_macro.h"
#include "ctr_type.h"
#include "evdns.h"
#include <vector>

CTR_NAMESPACE_BEGIN

// 地址信息
class addr_info
{
public:
    typedef union{
        sockaddr generic;
        sockaddr_in ipv4;
        sockaddr_in6 ipv6;
    }addr_type;
    
    addr_info(void);
    addr_info(sockaddr* addr, socklen_t len);
    virtual ~addr_info(void);
    
    // 获取地址结构体
    const sockaddr& addr(void) const;
    // 获取地址长度
    socklen_t addr_len(void) const;
    // 获取地址协议簇
    sa_family_t family(void) const;
    // 获取地址字符串
    ctr_strptr ip(void) const;
    // 设置端口
    void set_port(in_port_t port);
private:
    addr_type info_;
    ctr_char ip_[128];
};

// 地址集合
class address
{
public:
    address(void);
    virtual ~address(void);
    
    // 初始化地址集合
    void init(evutil_addrinfo* addr);
    // 清理地址集合
    void clear(void);
    // 地址集合数量
    ctr_int32 size(void);
    // 获取地址集合中的地址
    addr_info& operator[](ctr_int32 index);
private:
    std::vector<addr_info> addr_vec_;
};

CTR_NAMESPACE_END

#endif // _CTR_ADDRESS_H_
