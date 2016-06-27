//
//  ctr_state.hpp 16-06-08
//

#ifndef _ctr_state_hpp_
#define _ctr_state_hpp_

#include "ctr_macro.h"

#include "ctr_state_base.hpp"

ctr_namespace_begin

class state_root;
class state_empty;

template<typename super = state_root, typename entry = state_empty>
class state : public state_base
{
public:
    typedef super super_type;
    typedef entry entry_type;
};

class state_root : public state<>{};

class state_empty : public state<>{};

ctr_namespace_end

#endif // _ctr_state_hpp_
