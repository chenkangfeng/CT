//
//  ctr_state_data.hpp 16-06-08
//

#ifndef _ctr_state_data_hpp_
#define _ctr_state_data_hpp_

#include "ctr_macro.h"

ctr_namespace_begin

class state_data
{
public:
    virtual ~state_data(void){}
    
    template<typename type>
    type* get_data(void)
    {
        return dynamic_cast<type*>(this);
    }
};

ctr_namespace_end

#endif // _ctr_state_data_hpp_
