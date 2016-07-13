//
//  ctr_dns.cpp 2016-07-04
//  DNS解析
//

#include "ctr_dns.h"
#include "event.h"
#include "evdns.h"

using namespace std;

CTR_NAMESPACE_BEGIN

static void init_domain(ctr_strptr& domain, evutil_addrinfo& hints)
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
    init_domain(domain, hints);
    ctr_bool ret = (evutil_getaddrinfo(domain, NULL, &hints, &result) == 0);
    if(ret){
        addr.init(result);
    }
    evutil_freeaddrinfo(result);
    return ret;
}

void dns::set_timeout(const timeval& time, const function<void(void)>& callback)
{
    dns_set_timeout ev(callback);
    ev.time = time;
    machine_.send_event(ev);
}

ctr_bool dns::asyn_parse(ctr_strptr domain, const function<void(address&)>& callback)
{
    ctr_bool ret = false;
    dns_asyn_parse_ready ev(callback);
    ev.domain = domain;
    if((ret = machine_.send_event(ev))){
        dns_asyn_parse_start ev;
        machine_.send_event(ev);
    }
    return ret;
}

dns_asyn::dns_asyn(void)
{
    time.tv_sec = 3;
    time.tv_usec = 0;
}

ctr_bool dns_asyn::event(dns_asyn_init& ev)
{
    machine = ev.machine;
    return true;
}

ctr_bool dns_asyn::event(dns_set_timeout& ev)
{
    memcpy(&time, &ev.time, sizeof(ev.time));
    timeout_callback = ev.timeout_callback;
    return true;
}

static void dns_timeout(evutil_socket_t, ctr_int16, void* param)
{
    if(param != NULL){
        state_machine* machine = (state_machine*)param;
        dns_asyn_parse_timeout ev;
        machine->send_event(ev);
    }
}

void dns_asyn_work::enter(void)
{
    ev_base = event_base_new();
    ev_dns_base = evdns_base_new(ev_base, 1);
    ev = evtimer_new(ev_base, dns_timeout, state_ptr<dns_asyn>()->machine);
}

void dns_asyn_work::exit(void)
{
    event_free(ev);
    evdns_base_free(ev_dns_base, 1);
    event_base_free(ev_base);
}

static void dns_callback(ctr_int32 error, evutil_addrinfo* result, void* param)
{
    if(param != NULL){
        state_machine* machine = (state_machine*)param;
        dns_asyn_parse_succeed ev;
        ev.error = error;
        ev.result = result;
        machine->send_event(ev);
    }
}

ctr_bool dns_asyn_work::event(dns_asyn_parse_ready& ev)
{
    parse_callback = ev.parse_callback;
    
    ctr_bool ret = false;
    evutil_addrinfo hints;
    init_domain(ev.domain, hints);
    ret = (evdns_getaddrinfo(ev_dns_base, ev.domain, NULL, &hints,
                             dns_callback, state_ptr<dns_asyn>()->machine) != NULL);
    if(ret){
        event_add(this->ev, &state_ptr<dns_asyn>()->time);
    }
    return ret;
}

static void dns_asyn_working(event_base* ev_base)
{
    event_base_dispatch(ev_base);
}

void dns_asyn_work_doing::exit(void)
{
    if(thread_.joinable()){
        thread_.join();
    }
    finish();
}

ctr_bool dns_asyn_work_doing::event(dns_asyn_parse_start& ev)
{
    thread_ = thread(dns_asyn_working, state_ptr<dns_asyn_work>()->ev_base);
    return true;
}

ctr_bool dns_asyn_work_doing::event(dns_asyn_parse_timeout& ev)
{
    event_base_loopexit(state_ptr<dns_asyn_work>()->ev_base, NULL);
    if(state_ptr<dns_asyn>()->timeout_callback){
        state_ptr<dns_asyn>()->timeout_callback();
    }
    return true;
}

ctr_bool dns_asyn_work_doing::event(dns_asyn_parse_succeed& ev)
{
    event_base_loopexit(state_ptr<dns_asyn_work>()->ev_base, NULL);
    ctr_bool ret = false;
    address addr;
    if(ev.error == 0){
        if(ev.result != NULL){
            ret = true;
            addr.init(ev.result);
            evutil_freeaddrinfo(ev.result);
        }
    }
    if(state_ptr<dns_asyn_work>()->parse_callback){
        state_ptr<dns_asyn_work>()->parse_callback(addr);
    }
    return ret;
}

CTR_NAMESPACE_END
