//
//  ctr_tcp.h 2016-6-22
//

#ifndef _ctr_tcp_h_
#define _ctr_tcp_h_

#include "ctr_macro.h"
#include "ctr_type.h"
#include "ctr_platform.h"
#include "event.h"
#include "listener.h"

#include "ctr_dns.h"

ctr_namespace_begin

class tcp
{
public:
    tcp(void);
    virtual ~tcp(void);
    
    ctr_bool listen(const addr_info& addr);
    ctr_bool connectr(const addr_info& addr);
private:
    event_config* ev_config_;
    event_base* ev_base_;
    evconnlistener* ev_listener_;
    
    dns dns_;
};

ctr_namespace_end

#endif // _ctr_tcp_h_
