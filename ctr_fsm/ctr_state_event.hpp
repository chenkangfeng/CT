//
//  ctr_state_event.hpp 16-06-08
//

#ifndef _ctr_state_event_hpp_
#define _ctr_state_event_hpp_

#include "ctr_macro.h"

ctr_namespace_begin

class state_machine;

class state_event
{
public:
    virtual ~state_event(void){}
    
    virtual void trigger(state_machine* machine) const{};
};

ctr_namespace_end

#endif // _ctr_state_event_hpp_
