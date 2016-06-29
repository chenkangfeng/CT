//
//  ctr_state.hpp 16-06-29
//

#ifndef _ctr_state_hpp_
#define _ctr_state_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <functional>

ctr_namespace_begin

class state_base
{
public:
    virtual ~state_base(void){}
    
    virtual void enter(void) = 0;
    virtual void exit(void) = 0;
    virtual void update(ctr_float32) = 0;
};

typedef std::function<void(void)> enter_func_type;
typedef std::function<void(void)> exit_func_type;
typedef std::function<void(ctr_float32)> update_func_type;

class state_root;
class state_empty;

template<typename self, typename super = state_root, typename entry = state_empty>
class state : public state_base
{
public:
    typedef super super_type;
    typedef entry entry_type;
    
    virtual ~state(void){}
    
    virtual void enter(void);
    virtual void exit(void);
    virtual void update(ctr_float32 dt);
    
    static enter_func_type enter_func;
    static exit_func_type exit_func;
    static update_func_type update_func;
};

template<typename self, typename super, typename entry>
enter_func_type state<self, super, entry>::enter_func = enter_func_type();
template<typename self, typename super, typename entry>
exit_func_type state<self, super, entry>::exit_func = exit_func_type();
template<typename self, typename super, typename entry>
update_func_type state<self, super, entry>::update_func = update_func_type();

template<typename self, typename super, typename entry>
void state<self, super, entry>::enter(void)
{
    if(enter_func){
        enter_func();
    }
}
template<typename self, typename super, typename entry>
void state<self, super, entry>::exit(void)
{
    if(exit_func){
        exit_func();
    }
}
template<typename self, typename super, typename entry>
void state<self, super, entry>::update(ctr_float32 dt)
{
    if(update_func){
        update_func(dt);
    }
}

class state_root : public state<state_root>{};
class state_empty : public state<state_empty>{};

ctr_namespace_end

#endif // _ctr_state_hpp_
