//
//  ctr_state_machine.hpp 16-07-02
//  状态机
//

#ifndef _CTR_STATE_MACHINE_HPP_
#define _CTR_STATE_MACHINE_HPP_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <list>
#include <mutex>

#include "ctr_state.hpp"
#include "ctr_state_event.hpp"

CTR_NAMESPACE_BEGIN

class state_machine
{
public:
    state_machine(void){}
    virtual ~state_machine(void)
    {_delete_state<state_nil>();}
    
    // 开启状态机
    template<typename entry>
    void start(void)
    {
        mutex_.lock();
        if(state_list_.empty()){
            if(state_type::is_equal<typename entry::super_type, state_nil>()){
                _alloc_state<entry>();
            }
        }
        mutex_.unlock();
    }
    // 更新状态机
    void update(ctr_float32 dt)
    {
        mutex_.lock();
        for(auto itor = state_list_.begin();
            itor != state_list_.end(); ++itor){
            (*itor)->update(dt);
        }
        mutex_.unlock();
    }
    // 判断状态的当前状态
    template<typename type>
    ctr_bool is_state(void)
    {return _get_state<type>() != NULL;}
    // 发送事件到状态机
    template<typename event>
    ctr_bool send_event(event& ev)
    {
        ctr_bool ret = false;
        mutex_.lock();
        typename event::state1_type* state1 = _get_state<typename event::state1_type>();
        if(state1 != NULL){
            if(state_event_type::is_refresh<event>()){
                ret = state1->event(ev);
            }else if(state_event_type::is_transition<event>() &&
                     state_type::is_equal<typename event::state1_type::super_type, typename event::state2_type::super_type>()){
                if(state1->event(ev)){
                    _delete_state<typename event::state1_type>();
                    _alloc_state<typename event::state2_type>();
                    typename event::state2_type* state2 = _get_state<typename event::state2_type>();
                    ret = (state2 != NULL && state2->event(ev));
                }
            }
        }
        mutex_.unlock();
        return ret;
    }
private:
    // 添加状态列表中的状态
    template<typename type>
    void _alloc_state(void)
    {
        if(!state_type::is_equal<type, state_nil>()){
            type* the_state = new type();
            if(state_list_.empty()){
                the_state->super_state_ = NULL;
            }else{
                the_state->super_state_ = dynamic_cast<typename type::super_type*>(state_list_.back());
            }
            the_state->enter();
            state_list_.push_back(the_state);
            if(!state_type::is_equal<typename type::entry_type, state_nil>()){
                _alloc_state<typename type::entry_type>();
            }
        }
    }
    // 删除状态列表中的状态
    template<typename type>
    void _delete_state(void)
    {
        if(!state_list_.empty()){
            state_base* the_state = state_list_.back();
            ctr_bool is_stop = (state_type::is_equal<type>(*the_state) ||
                                state_type::is_equal<state_nil>(*the_state));
            the_state->exit();
            state_list_.pop_back();
            delete the_state;
            if(!is_stop){
                _delete_state<type>();
            }
        }
    }
    // 获取状态列表中的状态
    template<typename type>
    type* _get_state(void)
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
    
    // 状态列表
    std::list<state_base*> state_list_;
    // 状态锁
    std::mutex mutex_;
};

CTR_NAMESPACE_END

#endif // _CTR_STATE_MACHINE_HPP_
