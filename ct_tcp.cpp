//
//  ct_tcp.cpp 2016-6-22
//

#include "ct_tcp.h"
#include "thread.h"

using namespace std;

ct_namespace_begin

tcp::tcp(void)
{
#if ct_target_platform == ct_platform_win32
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
#if ct_target_platform == ct_platform_win32
#else
	event_config_free(ev_config_);
	event_base_free(ev_base_);
#endif
}

void tcp_accepted(evconnlistener* linstener, evutil_socket_t accept_socket,
                  sockaddr* addr, ct_int32 sock_len, void* param)
{
    
}

ct_bool tcp::listen(const addr_info& addr)
{
    ev_listener_ = evconnlistener_new_bind(ev_base_, tcp_accepted, NULL,
                                           LEV_OPT_THREADSAFE|LEV_OPT_REUSEABLE, 5, &addr.addr(), addr.addr_len());
    return (ev_listener_ != NULL);
}

ct_bool tcp::connect(const addr_info& addr)
{
    return false;
}

ct_namespace_end
