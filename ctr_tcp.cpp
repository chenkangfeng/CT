//
//  ctr_tcp.cpp 2016-6-22
//

#include "ctr_tcp.h"
#include "thread.h"

using namespace std;

ctr_namespace_begin

tcp::tcp(void)
{
#if ctr_target_platform == ctr_platform_win32
#else
    evthread_use_pthreads();
    ev_config_ = event_config_new();
    event_config_set_flag(ev_config_, EVENT_BASE_FLAG_EPOLL_USE_CHANGELIST);
    ev_base_ = event_base_new_with_config(ev_config_);
#endif
    ev_listener_ = NULL;
}

tcp::~tcp(void)
{
#if ctr_target_platform == ctr_platform_win32
#else
	event_config_free(ev_config_);
	event_base_free(ev_base_);
#endif
}

void tcp_accepted(evconnlistener* linstener, evutil_socket_t accept_socket,
                  sockaddr* addr, ctr_int32 sock_len, void* param)
{
    
}

ctr_bool tcp::listen(const addr_info& addr)
{
    ev_listener_ = evconnlistener_new_bind(ev_base_, tcp_accepted, NULL,
                                           LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE, 5, &addr.addr(), addr.addr_len());
    return (ev_listener_ != NULL);
}

ctr_bool tcp::connectr(const addr_info& addr)
{
    return false;
}

ctr_namespace_end
