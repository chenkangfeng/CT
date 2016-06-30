//
//  ctr_state_machine.hpp 16-06-29
//

#ifndef _ctr_state_machine_hpp_
#define _ctr_state_machine_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <list>

#include "ctr_state.hpp"

ctr_namespace_begin

class state_machine
{
public:
    state_machine(void){}
    virtual ~state_machine(void)
    {
        _delete_state<state_empty>();
    }
    
    template<typename entry>
    ctr_bool start(void);
    void update(ctr_float32 dt)
    {
        for(auto itor = state_list_.begin();
            itor != state_list_.end(); ++itor){
            (*itor)->update(dt);
        }
    }
    template<typename type>
    ctr_bool is_state(void);
    template<typename event, typename from, typename to>
    ctr_bool transition(event& ev);
    template<typename event>
    ctr_bool send_event(event& ev);
private:
    template<typename type>
    ctr_bool _alloc_state(void);
    template<typename type>
    ctr_bool _delete_state(void);
    template<typename type>
    type* _get_state(void);
    
    std::list<state_base*> state_list_;
};

template<typename entry>
ctr_bool state_machine::start(void)
{
    ctr_bool ret = false;
    if(state_list_.empty()){
        if(state_type::is_equal<typename entry::super_type, state_empty>()){
            ret = _alloc_state<entry>();
        }
    }
    return ret;
}
template<typename type>
ctr_bool state_machine::is_state(void)
{
    return _get_state<type>() != NULL;
}
template<typename event, typename from, typename to>
ctr_bool state_machine::transition(event& ev)
{
    ctr_bool ret = false;
    from* from_state = _get_state<from>();
    if(from_state != NULL){
        if(state_type::is_equal<from, to>()){
            ret = from_state->event(ev);
        }else{
            if(state_type::is_equal<typename from::super_type, typename to::super_type>()){
                if(from_state->event(ev)){
                    if(_delete_state<from>()){
                        if(_alloc_state<to>()){
                            to* to_state = _get_state<to>();
                            ret = (to_state != NULL && to_state->event(ev));
                        }else{
                            _delete_state<to>();
                            _alloc_state<from>();
                        }
                    }
                }
            }
        }
    }
    return ret;
}
template<typename event>
ctr_bool state_machine::send_event(event& ev)
{
    return transition<event, typename event::from_type, typename event::to_type>(ev);
}

template<typename type>
ctr_bool state_machine::_alloc_state(void)
{
    ctr_bool ret = false;
    if(!state_type::is_equal<type, state_empty>()){
        type* the_state = new type();
        if(state_list_.empty()){
            the_state->super_state_ = NULL;
        }else{
            the_state->super_state_ = dynamic_cast<typename type::super_type*>(state_list_.back());
        }
        ret = the_state->enter();
        if(ret){
            state_list_.push_back(the_state);
            if(!state_type::is_equal<typename type::entry_type, state_empty>()){
                ret = (ret && _alloc_state<typename type::entry_type>());
            }
        }else{
            delete the_state;
        }
    }
    return ret;
}
template<typename type>
ctr_bool state_machine::_delete_state(void)
{
    ctr_bool ret = false;
    if(!state_list_.empty()){
        state_base* the_state = state_list_.back();
        ctr_bool is_stop = (state_type::is_equal<type>(*the_state) ||
                            state_type::is_equal<state_empty>(*the_state));
        ret = the_state->exit();
        if(ret){
            state_list_.pop_back();
            delete the_state;
            if(!is_stop){
                ret = (ret && _delete_state<type>());
            }
        }
    }
    return ret;
}
template<typename type>
type* state_machine::_get_state(void)
{
    type* ret = NULL;
    for(auto itor = state_list_.rbegin();
        itor != state_list_.rend(); ++itor){
        if(state_type::is_equal<type>(**itor)){
            ret = dynamic_cast<type*>(*itor);
            break;
        }
    }
    return ret;
}

ctr_namespace_end

#endif // _ctr_state_machine_hpp_
