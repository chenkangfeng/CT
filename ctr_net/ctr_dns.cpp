//
//  ctr_dns.cpp 2016-07-04
//  DNS解析
//

#include "ctr_dns.h"
#include "event.h"
#include "evdns.h"

using namespace std;

CTR_NAMESPACE_BEGIN

dns::dns(void)
{
    time_.tv_sec = 3;
    time_.tv_usec = 0;
    
    ev_base_ = event_base_new();
    ev_dns_base_ = evdns_base_new(ev_base_, 1);
}

dns::~dns(void)
{
    if(thread_.joinable()){
        thread_.join();
    }
    
    evdns_base_free(ev_dns_base_, 1);
    event_base_free(ev_base_);
}

ctr_bool dns::parse(ctr_strptr domain, address& addr)
{
    evutil_addrinfo hints, *result;
    _init_domain(domain, hints);
    ctr_bool ret = (evutil_getaddrinfo(domain, NULL, &hints, &result) == 0);
    if(ret){
        addr.init(result);
    }
    evutil_freeaddrinfo(result);
    return ret;
}

void dns::set_timeout(const timeval& time, const function<void(void)>& callback)
{
    memcpy(&time_, &time, sizeof(time));
    timeout_callback_ = callback;
}

void dns::set_error(const function<void(void)>& callback)
{
    error_callback_ = callback;
}

ctr_bool dns::asyn_parse(ctr_strptr domain, const function<void(address&)>& callback)
{
    parse_callback_ = callback;
    
    ctr_bool ret = false;
    if((ret = !thread_.joinable())){
        thread_ = thread(bind(&dns::_asyn_parse_process, this, placeholders::_1), domain);
    }
    return ret;
}

void dns::asyn_parse_callback(evutil_addrinfo* result)
{
    addr_.init(result);
    event_base_loopexit(ev_base_, NULL);
}

void dns::_init_domain(ctr_strptr& domain, evutil_addrinfo& hints)
{
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_CANONNAME;
    if(strncasecmp("tcp://", domain, 6) == 0){
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
    }else if(strncasecmp("udp://", domain, 6) == 0){
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
    }
    
    ctr_strptr sub_domain = strstr(domain, "://");
    if(sub_domain != NULL){
        domain = sub_domain + 3;
    }
}

static void dns_callback(ctr_int32 error, evutil_addrinfo* result, void* param)
{
    if(error == 0 && result != NULL && param != NULL){
        dns* dns_ptr = (dns*)param;
        dns_ptr->asyn_parse_callback(result);
        evutil_freeaddrinfo(result);
    }
}

void dns::_asyn_parse_process(ctr_strptr domain)
{
    evutil_addrinfo hints;
    _init_domain(domain, hints);
    addr_.clear();
    
    if(evdns_getaddrinfo(ev_dns_base_, domain, NULL, &hints, dns_callback, this)){
        event_base_loopexit(ev_base_, &time_);
        event_base_dispatch(ev_base_);
        if(addr_.size() > 0){
            if(parse_callback_){
                parse_callback_(addr_);
            }
        }else{
            if(timeout_callback_){
                timeout_callback_();
            }
        }
    }else{
        if(error_callback_){
            error_callback_();
        }
    }
}

CTR_NAMESPACE_END
