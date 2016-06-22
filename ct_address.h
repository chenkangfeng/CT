//
//  ct_address.h 2016-6-22
//

#ifndef _ct_address_h_
#define _ct_address_h_

#include "ct_macro.h"
#include "ct_type.h"
#include "evdns.h"
#include <vector>

ct_namespace_begin

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
    
    sa_family_t family(void);
    ct_strptr ip(void);
private:
    addr_type info_;
    ct_char ip_[128];
};

class address
{
public:
    address(void);
    virtual ~address(void);
    
    void init(evutil_addrinfo* addr);
    ct_int32 size(void);
    addr_info& operator[](ct_int32 index);
private:
    std::vector<addr_info> addr_vec_;
};

ct_namespace_end

#endif // _ct_address_h_
