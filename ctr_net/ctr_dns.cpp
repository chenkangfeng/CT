//
//  ctr_dns.cpp 2016-6-22
//

#include "ctr_dns.h"
#include "event.h"
#include "evdns.h"
#include <string.h>
#include <functional>
#include <thread>

using namespace std;

ctr_namespace_begin

class dns_asyn;
class dns_asyn_stop;
class dns_asyn_work;
class dns_asyn_work_doing;
class dns_asyn_work_succeed;
class dns_asyn_work_failed;

class dns_asyn_init : public state_event
{
    state_transition(dns_asyn, dns_asyn);
    
    state_machine* machine;
};

class dns_set_timeout : public state_event
{
public:
    state_transition(dns_asyn, dns_asyn);
    
    timeval time;
    std::function<void(void)> callback;
};

class dns_asyn_parse : public state_event
{
public:
    state_transition(dns_asyn_stop, dns_asyn_work);
    
    ctr_strptr domain;
    function<void(address&)> callback;
};

class dns_asyn_parse_succeed : public state_event
{
public:
    state_transition(dns_asyn_work_doing, dns_asyn_work_succeed);
    
    evutil_addrinfo* result;
};

class dns_asyn_parse_failed : public state_event
{
public:
    state_transition(dns_asyn_work_doing, dns_asyn_work_failed);
};

class dns_asyn : public state<state_empty, dns_asyn_stop>
{
public:
    dns_asyn(void)
    {
        time.tv_sec = 3;
        time.tv_usec = 0;
    }
    ctr_bool event(dns_asyn_init& ev)
    {
        machine = ev.machine;
        return true;
    }
    ctr_bool event(dns_set_timeout& ev)
    {
        memcpy(&time, &ev.time, sizeof(ev.time));
        timeout_callback = ev.callback;
        return true;
    }
    
    state_machine* machine;
    
    timeval time;
    std::function<void(void)> timeout_callback;
};

class dns_asyn_stop : public state<dns_asyn>{};

static void dns_timeout(evutil_socket_t, ctr_int16, void* param)
{
    if(param != NULL){
        state_machine* machine = (state_machine*)param;
        dns_asyn_parse_failed ev;
        machine->send_event(ev);
    }
}

static void dns_callback(ctr_int32 error, evutil_addrinfo* result, void* param)
{
    if(param != NULL){
        state_machine* machine = (state_machine*)param;
        dns_asyn_parse_succeed ev;
        ev.result = result;
        machine->send_event(ev);
        evutil_freeaddrinfo(result);
    }
}

class dns_asyn_work : public state<dns_asyn, dns_asyn_work_doing>
{
public:
    dns_asyn_work(void)
    {
        ev_base = NULL;
        ev_dns_base = NULL;
        ev = NULL;
    }
    
    virtual ctr_bool enter(void)
    {
        ev_base = event_base_new();
        ev_dns_base = evdns_base_new(ev_base, 1);
        ev = evtimer_new(ev_base, dns_timeout, super_state_->machine);
        return true;
    }
    virtual ctr_bool exit(void)
    {
        event_free(ev);
        evdns_base_free(ev_dns_base, 1);
        event_base_free(ev_base);
        return true;
    }
    ctr_bool event(dns_asyn_parse& ev)
    {
        domain = ev.domain;
        parse_callback = ev.callback;
        return true;
    }
    
    struct event_base* ev_base;
    struct evdns_base* ev_dns_base;
    struct event* ev;
    
    ctr_strptr domain;
    std::function<void(address&)> parse_callback;
};

static void dns_working(event_base* ev_base)
{
    event_base_dispatch(ev_base);
}

class dns_asyn_work_doing : public state<dns_asyn_work>
{
public:
    virtual ctr_bool enter(void)
    {
        ctr_bool ret = false;
        evutil_addrinfo hints;
        //_init_domain(ev.domain, hints);
        ret = (evdns_getaddrinfo(state_ptr<dns_asyn_work>()->ev_dns_base, state_ptr<dns_asyn_work>()->domain, NULL, &hints,
                                 dns_callback, state_ptr<dns_asyn>()->machine) != NULL);
        if(ret){
            event_add(state_ptr<dns_asyn_work>()->ev, &state_ptr<dns_asyn>()->time);
            working_thread = thread(dns_working, state_ptr<dns_asyn_work>()->ev_base);
        }
        return ret;
    }
    
    std::thread working_thread;
};

class dns_asyn_work_succeed : public state<dns_asyn_work>
{
public:
    virtual ctr_bool enter(void)
    {
        return true;
    }
};

class dns_asyn_work_failed : public state<dns_asyn_work>
{

};

dns::dns(void)
{
    machine_.start<dns_asyn>();
    dns_asyn_init ev;
    ev.machine = &machine_;
    machine_.send_event(ev);
}

dns::~dns(void)
{
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
    dns_set_timeout ev;
    ev.time = time;
    ev.callback = callback;
    machine_.send_event(ev);
}

ctr_bool dns::asyn_parse(ctr_strptr domain, const function<void(address&)>& callback)
{
    dns_asyn_parse ev;
    ev.domain = domain;
    ev.callback = callback;
    return machine_.send_event(ev);
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
