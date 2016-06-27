//
//  ctr_state_base.hpp 16-06-08
//

#ifndef _ctr_state_base_hpp_
#define _ctr_state_base_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"

#include "ctr_state_function.hpp"

ctr_namespace_begin

class state_data;

class state_base
{
public:
    virtual ~state_base(void){}
    
    virtual void enter(state_data* data)
    {
        if(function.enter){
            function.enter(data);
        }
    }
    virtual void exit(state_data* data)
    {
        if(function.exit){
            function.exit(data);
        }
    }
    virtual void update(ctr_float32 dt, state_data* data)
    {
        if(function.update){
            function.update(dt, data);
        }
    }
    
    state_function function;
};

ctr_namespace_end

#endif // _ctr_state_base_hpp_
