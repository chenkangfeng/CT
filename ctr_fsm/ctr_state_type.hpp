//
//  ctr_state_type.hpp 16-06-29
//

#ifndef _ctr_state_type_hpp_
#define _ctr_state_type_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <typeinfo>

ctr_namespace_begin

class state_base;

class state_type
{
public:
    template<typename type>
    static ctr_bool is_equal(const state_base& state);
    template<typename type1, typename type2>
    static ctr_bool is_equal(void);
    static ctr_bool is_equal(const state_base& state1, const state_base& state2);
};

template<typename type>
ctr_bool state_type::is_equal(const state_base& state)
{
    return typeid(type) == typeid(state);
}
template<typename type1, typename type2>
ctr_bool state_type::is_equal(void)
{
    return typeid(type1) == typeid(type2);
}
ctr_bool state_type::is_equal(const state_base& state1, const state_base& state2)
{
    return typeid(state1) == typeid(state2);
}

ctr_namespace_end

#endif // _ctr_state_type_hpp_
