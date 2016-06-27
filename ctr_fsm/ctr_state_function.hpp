//
//  ctr_state_function.hpp 16-06-08
//

#ifndef _ctr_state_function_hpp_
#define _ctr_state_function_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <functional>

ctr_namespace_begin

class state_data;

class state_function
{
public:
    std::function<void(state_data*)> enter;
    std::function<void(state_data*)> exit;
    std::function<void(ctr_float32, state_data*)> update;
};

ctr_namespace_end

#endif // _ctr_state_function_hpp_
