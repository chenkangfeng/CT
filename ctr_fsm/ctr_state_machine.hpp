//
//  ctr_state_machine.hpp 16-06-29
//

#ifndef _ctr_state_machine_hpp_
#define _ctr_state_machine_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <list>

#include "ctr_state.hpp"
#include "ctr_state_type.hpp"

ctr_namespace_begin

class state_machine
{
public:
    state_machine(void){}
    virtual ~state_machine(void);
    
    template<typename entry>
    void start(void);
    void update(ctr_float32 dt);
    template<typename type>
    ctr_bool is_state(void);
    template<typename from, typename to>
    void transition(void);
private:
    template<typename type>
    void _alloc_state(void);
    template<typename type>
    void _delete_state(void);
    
    std::list<state_base*> state_list_;
};

state_machine::~state_machine(void)
{
    _delete_state<state_root>();
}

template<typename entry>
void state_machine::start(void)
{
    _alloc_state<entry>();
}
void state_machine::update(ctr_float32 dt)
{
    for(auto the_state:state_list_){
        the_state->update(dt);
    }
}
template<typename type>
ctr_bool state_machine::is_state(void)
{
    ctr_bool ret = false;
    for(auto itor = state_list_.rbegin();
        itor != state_list_.rend(); ++itor){
        if((ret = state_type::is_equal<type>(**itor))){
            break;
        }
    }
    return ret;
}
template<typename from, typename to>
void state_machine::transition(void)
{
    if(is_state<from>() &&
       state_type::is_equal<typename from::super_type,
       typename to::super_type>()){
        _delete_state<from>();
        _alloc_state<to>();
    }
}

template<typename type>
void state_machine::_alloc_state(void)
{
    if(!state_type::is_equal<type, state_empty>()){
        state_base* the_state = new type();
        the_state->enter();
        state_list_.push_back(the_state);
        if(!state_type::is_equal<type, typename type::enter_type>()){
            _alloc_state<typename type::entry_type>();
        }
    }
}
template<typename type>
void state_machine::_delete_state(void)
{
    if(!state_list_.empty()){
        state_base* the_state = state_list_.back();
        ctr_bool is_stop = (state_type::is_equal<type>(*the_state) ||
                            state_type::is_equal<state_root>(*the_state));
        the_state->exit();
        state_list_.pop_back();
        delete the_state;
        if(!is_stop){
            _delete_state<type>();
        }
    }
}

ctr_namespace_end

#endif // _ctr_state_machine_hpp_
