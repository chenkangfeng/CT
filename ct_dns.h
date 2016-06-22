//
//  ct_dns.h 2016-6-22
//

#ifndef _ct_dns_h_
#define _ct_dns_h_

#include "ct_macro.h"
#include "ct_type.h"
#include "event.h"
#include "evdns.h"
#include <functional>
#include <thread>

#include "ct_address.h"

ct_namespace_begin

class dns
{
public:
    dns(void);
    virtual ~dns(void);
    
    ct_bool parse(ct_strptr domain, address& addr);
    
    void set_timeout(const timeval& time, const std::function<void(void)>& callback);
    ct_bool asyn_parse(ct_strptr domain, const std::function<void(address&)>& callback);
    
    void parse_callback(address& addr);
    void timeout_callback(void);
private:
    void _enable_asyn(ct_bool enable);
    void _init_domain(ct_strptr& domain, evutil_addrinfo& hints);
    
    ct_bool enable_asyn_;
    ct_bool is_working_;
    
    event_base* ev_base_;
    evdns_base* ev_dns_base_;
    event* ev_;
    
    timeval time_;
    std::function<void(void)> timeout_callback_;
    
    std::mutex mutex_;
    std::thread thread_;
    std::function<void(address&)> parse_callback_;
};

ct_namespace_end

#endif // _ct_dns_h_
