//
//  ctr_address.h 2016-6-22
//

#ifndef _ctr_address_h_
#define _ctr_address_h_

#include "ctr_macro.h"
#include "ctr_type.h"
#include "evdns.h"
#include <vector>

ctr_namespace_begin

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
    
    const sockaddr& addr(void) const;
    socklen_t addr_len(void) const;
    sa_family_t family(void) const;
    ctr_strptr ip(void) const;
    void set_port(in_port_t port);
private:
    addr_type info_;
    ctr_char ip_[128];
};

class address
{
public:
    address(void);
    virtual ~address(void);
    
    void init(evutil_addrinfo* addr);
    ctr_int32 size(void);
    addr_info& operator[](ctr_int32 index);
private:
    std::vector<addr_info> addr_vec_;
};

ctr_namespace_end

#endif // _ctr_address_h_
