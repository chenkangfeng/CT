//
//  ctr_state.hpp 16-06-29
//

#ifndef _ctr_state_hpp_
#define _ctr_state_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <functional>
#include <typeinfo>

#define state_transition(state1, state2) \
public:                                  \
typedef state1 from_type;                \
typedef state2 to_type;

ctr_namespace_begin

class state_machine;
class state_empty;

class state_event
{
public:
    state_transition(state_empty, state_empty)
    
    virtual ~state_event(void){}
};

class state_base
{
public:
    virtual ~state_base(void){}
    
    virtual ctr_bool enter(void){return true;}
    virtual ctr_bool exit(void){return true;}
    virtual ctr_bool update(ctr_float32){return true;}
    ctr_bool event(state_event&){return true;}
};

class state_type
{
public:
    template<typename type>
    static ctr_bool is_equal(const state_base& state);
    template<typename type1, typename type2>
    static ctr_bool is_equal(void);
    static ctr_bool is_equal(const state_base& state1, const state_base& state2)
    {
        return typeid(state1) == typeid(state2);
    }
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

template<typename super = state_empty, typename entry = state_empty>
class state : public state_base
{
    friend class state_machine;
public:
    typedef super super_type;
    typedef entry entry_type;
    
    state(void):super_state_(NULL){}
    virtual ~state(void){}

    template<typename type>
    type* state_ptr(void)
    {
        type* ret = NULL;
        if(super_state_ != NULL){
            if(state_type::is_equal<type>(*this)){
                ret = dynamic_cast<type*>(this);
            }else{
                ret = super_state_->template state_ptr<type>();
            }
        }
        return ret;
    }
protected:
    super* super_state_;
};

class state_empty : public state<>{};

template<typename self, typename super = state_empty, typename entry = state_empty>
class state_ex : public state<super, entry>
{
public:
    typedef std::function<ctr_bool(self&)> enter_func_type;
    typedef std::function<ctr_bool(self&)> exit_func_type;
    typedef std::function<ctr_bool(self&, ctr_float32)> update_func_type;
    
    typedef super super_type;
    typedef entry entry_type;
    
    virtual ~state_ex(void){}
    
    virtual ctr_bool enter(void);
    virtual ctr_bool exit(void);
    virtual ctr_bool update(ctr_float32 dt);
    
    static enter_func_type enter_func;
    static exit_func_type exit_func;
    static update_func_type update_func;
};

template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::enter_func_type state_ex<self, super, entry>::enter_func = enter_func_type();
template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::exit_func_type state_ex<self, super, entry>::exit_func = exit_func_type();
template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::update_func_type state_ex<self, super, entry>::update_func = update_func_type();

template<typename self, typename super, typename entry>
ctr_bool state_ex<self, super, entry>::enter(void)
{
    ctr_bool ret = true;
    if(enter_func){
        ret = enter_func(*dynamic_cast<self*>(this));
    }
    return ret;
};
template<typename self, typename super, typename entry>
ctr_bool state_ex<self, super, entry>::exit(void)
{
    ctr_bool ret = true;
    if(exit_func){
        ret = exit_func(*dynamic_cast<self*>(this));
    }
    return ret;
};
template<typename self, typename super, typename entry>
ctr_bool state_ex<self, super, entry>::update(ctr_float32 dt)
{
    ctr_bool ret = true;
    if(update_func){
        ret = update_func(*dynamic_cast<self*>(this), dt);
    }
    return ret;
};

ctr_namespace_end

#endif // _ctr_state_hpp_
