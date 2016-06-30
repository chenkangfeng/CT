//
//  ctr_dns.h 2016-6-22
//

#ifndef _ctr_dns_h_
#define _ctr_dns_h_

#include "ctr_macro.h"
#include "ctr_type.h"

#include "ctr_address.h"
#include "ctr_state_machine.hpp"

ctr_namespace_begin

class dns
{
public:
    dns(void);
    virtual ~dns(void);
    
    ctr_bool parse(ctr_strptr domain, address& addr);
    
    void set_timeout(const timeval& time, const std::function<void(void)>& callback);
    ctr_bool asyn_parse(ctr_strptr domain, const std::function<void(address&)>& callback);
private:
    void _init_domain(ctr_strptr& domain, evutil_addrinfo& hints);
    
    state_machine machine_;
};

ctr_namespace_end

#endif // _ctr_dns_h_
