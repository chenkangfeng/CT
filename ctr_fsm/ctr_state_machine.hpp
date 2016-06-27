//
//  ctr_state_machine.hpp 16-06-08
//

#ifndef _ctr_state_machine_hpp_
#define _ctr_state_machine_hpp_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <list>
#include <map>
#include <string>

#include "ctr_state.hpp"
#include "ctr_state_event.hpp"
#include "ctr_state_type.hpp"

ctr_namespace_begin

class state_machine
{
public:
    state_machine(state_data* data):data_(data){}
    virtual ~state_machine(void)
    {
        _delete_state<state_root>();
    }
    
    state_data* get_data(void)
    {
        return data_;
    }
    
    template<typename entry>
    void start(void)
    {
        _alloc_state<entry>();
    }
    
    void update(ctr_float32 dt)
    {
        for(auto the_state:working_states_){
            the_state->update(dt, data_);
        }
    }
    
    template<typename type>
    ctr_bool is_state(void)
    {
        ctr_bool ret = false;
        for(auto itor = working_states_.rbegin();
            itor != working_states_.rend(); ++itor){
            if((ret = state_type::is_equal<type>(**itor))){
                break;
            }
        }
        return ret;
    }
    
    template<typename from, typename to>
    void transition(void)
    {
        if(is_state<from>() &&
           state_type::is_equal<typename from::super_type,
                                typename to::super_type>()){
            _delete_state<from>();
            _alloc_state<to>();
        }
    }
    
    void send_event(const state_event& event)
    {
        event.trigger(this);
    }
    
//    template<typename Type>
//    void set_state_func(const StateCallback& callback)
//    {
//        stateCallback_[typeid(Type).name()] = callback;
//    }
private:
    template<typename type>
    void _alloc_state(void)
    {
        if(!state_type::is_equal<type, state_empty>()){
            state_base* the_state = new type();
            the_state->enter(data_);
            working_states_.push_back(the_state);
            if(!state_type::is_equal<type, typename type::enter_type>()){
                _alloc_state<typename type::entry_type>();
            }
        }
    }
    
    template<typename type>
    void _delete_state(void)
    {
        if(!working_states_.empty()){
            state_base* the_state = working_states_.back();
            ctr_bool is_stop = (state_type::is_equal<type>(*the_state) ||
                                state_type::is_equal<state_root>(*the_state));
            the_state->exit(data_);
            working_states_.pop_back();
            delete the_state;
            if(!is_stop){
                _delete_state<type>();
            }
        }
    }
    
    std::list<state_base*> working_states_;
    state_data* data_;
};

ctr_namespace_end

#endif // _ctr_state_machine_hpp_
