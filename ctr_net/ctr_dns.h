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
    ctr_bool parse(ctr_strptr domain, address& addr);
    // 设置超时事件和回掉函数
    void set_timeout(const timeval& time, const std::function<void(void)>& callback);
    // 异步DNS解析
    ctr_bool asyn_parse(ctr_strptr domain, const std::function<void(address&)>& callback);
private:
    // 异步解析状态机
    state_machine machine_;
    // 异步解析线程
    std::thread thread_;
};


class dns_asyn;
class dns_asyn_wait;
class dns_asyn_work;
class dns_asyn_work_ready;
class dns_asyn_work_doing;

// DNS异步解析初始化事件
class dns_asyn_init : public state_refresh<dns_asyn>
{
public:
    state_machine* machine;
};

// DNS异步解析设置超时事件
class dns_set_timeout : public state_refresh<dns_asyn>
{
public:
    dns_set_timeout(const std::function<void(void)>& callback):
    timeout_callback(callback){}
    
    timeval time;
    const std::function<void(void)>& timeout_callback;
};

// DNS异步解析进入准备事件
class dns_asyn_parse_ready : public state_transition<dns_asyn_wait, dns_asyn_work>
{
public:
    dns_asyn_parse_ready(const std::function<void(address&)>& callback):
    parse_callback(callback){}
    
    ctr_strptr domain;
    const std::function<void(address&)>& parse_callback;
};

// DNS异步解析开始解析事件
class dns_asyn_parse_start : public state_transition<dns_asyn_work_ready, dns_asyn_work_doing>{};

// DNS异步解析超时事件
class dns_asyn_parse_timeout : public state_transition<dns_asyn_work_doing, dns_asyn_work_ready>{};

// DNS异步解析成功事件
class dns_asyn_parse_succeed : public state_transition<dns_asyn_work_doing, dns_asyn_work_ready>
{
public:
    ctr_int32 error;
    evutil_addrinfo* result;
};

// DNS异步解析基状态
class dns_asyn : public state<state_nil, dns_asyn_wait>
{
public:
    dns_asyn(void);
    
    ctr_bool event(dns_asyn_init& ev);
    ctr_bool event(dns_set_timeout& ev);
    
    state_machine* machine;
    
    timeval time;
    std::function<void(void)> timeout_callback;
};

// DNS异步解析等待状态
class dns_asyn_wait : public state<dns_asyn>{};

// DNS异步解析工作状态
class dns_asyn_work : public state<dns_asyn, dns_asyn_work_ready>
{
public:
    virtual void enter(void);
    virtual void exit(void);
    
    ctr_bool event(dns_asyn_parse_ready& ev);
    
    struct event_base* ev_base;
    struct evdns_base* ev_dns_base;
    struct event* ev;
    
    std::function<void(address&)> parse_callback;
};

// DNS异步解析装备状态
class dns_asyn_work_ready : public state<dns_asyn_work>
{
public:
    ctr_bool event(state_event& ev);
    ctr_bool event(dns_asyn_parse_timeout& ev);
    ctr_bool event(dns_asyn_parse_succeed& ev);
};

// DNS异步解析执行状态
class dns_asyn_work_doing : public state<dns_asyn_work>
{
public:
    ctr_bool event(state_event& ev);
    ctr_bool event(dns_asyn_parse_start& ev);
};

CTR_NAMESPACE_END

#endif // _CTR_DNS_H_
