//
//  ct_tcp.h 2016-6-22
//

#ifndef _ct_tcp_h_
#define _ct_tcp_h_

#include "ct_macro.h"
#include "ct_type.h"
#include "event.h"

ct_namespace_begin

class tcp
{
public:
    tcp(void);
    virtual ~tcp(void);
    
    bool listen(ct_strptr domain, in_port_t port);
    bool connect(ct_strptr domain, in_port_t port);
};

ct_namespace_end

#endif // _ct_tcp_h_
