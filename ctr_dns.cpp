//
//  ctr_dns.cpp 2016-6-22
//

#include "ctr_dns.h"
#include <string.h>

using namespace std;

ctr_namespace_begin

dns::dns(void)
{
    enable_asyn_ = false;
    is_working_ = false;
    
    ev_base_ = NULL;
    ev_dns_base_ = NULL;
    ev_ = NULL;
    
    time_.tv_sec = 3;
    time_.tv_usec = 0;
}

dns::~dns(void)
{
    if(thread_.joinable()){
        thread_.join();
    }
    _enable_asyn(false);
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

void dns::set_timeout(const timeval& time, const functrion<void(void)>& callback)
{
    memcpy(&time_, &time, sizeof(time));
    timeout_callback_ = callback;
}

static void dns_callback(ctr_int32 error, evutil_addrinfo* result, void* param)
{
    if(param != NULL){
        dns* dns_ins = (dns*)param;
        address addr;
        if(error == 0){
            addr.init(result);
        }
        dns_ins->parse_callback(addr);
        evutil_freeaddrinfo(result);
    }
}

static void dns_working(event_base* ev_base)
{
    event_base_dispatch(ev_base);
}

ctr_bool dns::asyn_parse(ctr_strptr domain, const functrion<void(address&)>& callback)
{
    if(is_working_){
        return false;
    }
    _enable_asyn(true);
    
    parse_callback_ = callback;
    
    evutil_addrinfo hints;
    _init_domain(domain, hints);
    ctr_bool ret = (evdns_getaddrinfo(ev_dns_base_, domain, NULL, &hints, dns_callback, this) != NULL);
    if(ret){
        is_working_ = true;
        event_add(ev_, &time_);
        thread_ = thread(dns_working, ev_base_);
    }
    return ret;
}

void dns::parse_callback(address& addr)
{
    if(mutex_.try_lock()){
        if(is_working_){
            if(parse_callback_){
                parse_callback_(addr);
            }
            event_base_loopexit(ev_base_, NULL);
            is_working_ = false;
        }
        mutex_.unlock();
    }
}

void dns::timeout_callback(void)
{
    if(mutex_.try_lock()){
        if(is_working_){
            if(timeout_callback_){
                timeout_callback_();
            }
            event_base_loopexit(ev_base_, NULL);
            is_working_ = false;
        }
        mutex_.unlock();
    }
}

static void dns_timeout(evutil_socket_t, ctr_int16, void* param)
{
    if(param != NULL){
        dns* dns_ins = (dns*)param;
        dns_ins->timeout_callback();
    }
}

void dns::_enable_asyn(ctr_bool enable)
{
    if(enable && enable != enable_asyn_){
        if(ev_base_ == NULL){
            ev_base_ = event_base_new();
        }
        if(ev_dns_base_ == NULL){
            ev_dns_base_ = evdns_base_new(ev_base_, 1);
        }
        if(ev_ == NULL){
            ev_ = evtimer_new(ev_base_, dns_timeout, this);
        }
        enable_asyn_ = enable;
    }else if(!enable && enable != enable_asyn_){
        if(ev_ != NULL){
            event_free(ev_);
            ev_ = NULL;
        }
        if(ev_dns_base_){
            evdns_base_free(ev_dns_base_, 1);
            ev_dns_base_ = NULL;
        }
        if(ev_base_ != NULL){
            event_base_free(ev_base_);
            ev_base_ = NULL;
        }
        enable_asyn_ = enable;
    }
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

ctr_namespace_end
