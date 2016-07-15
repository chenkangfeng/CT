//
//  ctr_dns.h 2016-07-04
//  DNS解析
//

#ifndef _CTR_DNS_H_
#define _CTR_DNS_H_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <thread>

#include "ctr_address.h"
#include "ctr_state_machine.hpp"

CTR_NAMESPACE_BEGIN

// DNS解析
class dns
{
public:
    dns(void);
    virtual ~dns(void);
    
    // 同步DNS解析
    static ctr_bool parse(ctr_strptr domain, address& addr);
    // 设置超时时间和回掉函数
    void set_timeout(const timeval& time, const std::function<void(void)>& callback);
    // 设置错误回掉函数
    void set_error(const std::function<void(void)>& callback);
    // 异步DNS解析
    ctr_bool asyn_parse(ctr_strptr domain, const std::function<void(address&)>& callback);
    // 异步DNS解析回掉
    void asyn_parse_callback(evutil_addrinfo* result);
private:
    // 初始化域名
    static void _init_domain(ctr_strptr& domain, evutil_addrinfo& hints);
    // 异步DNS解析过程
    void _asyn_parse_process(ctr_strptr domain);
    
    timeval time_;
    std::function<void(void)> timeout_callback_;
    std::function<void(void)> error_callback_;
    std::function<void(address&)> parse_callback_;
    
    std::thread thread_;
    
    address addr_;
    
    event_base* ev_base_;
    evdns_base* ev_dns_base_;
};

CTR_NAMESPACE_END

#endif // _CTR_DNS_H_
