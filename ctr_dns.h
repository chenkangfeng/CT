//
//  ctr_dns.h 2016-6-22
//

#ifndef _ctr_dns_h_
#define _ctr_dns_h_

#include "ctr_macro.h"
#include "ctr_type.h"
#include "event.h"
#include "evdns.h"
#include <functional>
#include <thread>

#include "ctr_address.h"

ctr_namespace_begin

class dns
{
public:
    dns(void);
    virtual ~dns(void);
    
    ctr_bool parse(ctr_strptr domain, address& addr);
    
    void set_timeout(const timeval& time, const std::function<void(void)>& callback);
    ctr_bool asyn_parse(ctr_strptr domain, const std::function<void(address&)>& callback);
    
    void parse_callback(address& addr);
    void timeout_callback(void);
private:
    void _enable_asyn(ctr_bool enable);
    void _init_domain(ctr_strptr& domain, evutil_addrinfo& hints);
    
    ctr_bool enable_asyn_;
    ctr_bool is_working_;
    
    event_base* ev_base_;
    evdns_base* ev_dns_base_;
    event* ev_;
    
    timeval time_;
    std::function<void(void)> timeout_callback_;
    
    std::mutex mutex_;
    std::thread thread_;
    std::function<void(address&)> parse_callback_;
};

ctr_namespace_end

#endif // _ctr_dns_h_
