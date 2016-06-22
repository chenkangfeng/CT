//
//  ct_tcp.h 2016-6-22
//

#ifndef _ct_tcp_h_
#define _ct_tcp_h_

#include "ct_macro.h"
#include "ct_type.h"
#include "ct_platform.h"
#include "event.h"
#include "listener.h"

#include "ct_dns.h"

ct_namespace_begin

class tcp
{
public:
    tcp(void);
    virtual ~tcp(void);
    
    ct_bool listen(const addr_info& addr);
    ct_bool connect(const addr_info& addr);
private:
    event_config* ev_config_;
    event_base* ev_base_;
    evconnlistener* ev_listener_;
    
    dns dns_;
};

ct_namespace_end

#endif // _ct_tcp_h_
