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

// 状态机
class state_machine
{
public:
    state_machine(void){}
    virtual ~state_machine(void){_delete_state<state_nil>();}
    
    // 开启状态机
    template<typename entry>
    void start(void)
    {
        if(state_list_.empty()){
            if(state_type::is_equal<typename entry::super_type, state_nil>()){
                _alloc_state<entry>();
            }
        }
    }
    // 更新状态机
    void update(ctr_float32 dt)
    {
        for(auto itor = state_list_.begin(); itor != state_list_.end(); ++itor){
            (*itor)->update(dt);
            if((*itor)->is_finish()){
                (*itor)->finished();
                break;
            }
        }
    }
    // 判断状态的当前状态
    template<typename type>
    ctr_bool is_state(void){return _get_state<type>() != NULL;}
    // 状态机状态转换
    template<typename type1, typename type2>
    void transition()
    {
        type1* state1 = _get_state<type1>();
        if(state1 != NULL){
            _transition<type1, type2>();
        }else{
            _alloc_state<type2>();
        }
    }
    // 发送事件到状态机
    template<typename event>
    ctr_bool send_event(event& ev)
    {
        ctr_bool ret = false;
        typename event::state1_type* state1 = _get_state<typename event::state1_type>();
        if(state1 != NULL){
            if(state_event_type::is_refresh<event>()){
                ret = state1->event(ev);
                if(state1->is_finish()){
                    _transition<typename event::state1_type, typename event::state1_type::next_type>();
                }
            }else if(state_event_type::is_transition<event>()){
                ret = state1->event(ev);
                if(state1->is_finish()){
                    _transition<typename event::state1_type, typename event::state1_type::next_type>();
                }else if(ret){
                    _transition<typename event::state1_type, typename event::state2_type>();
                    typename event::state2_type* state2 = _get_state<typename event::state2_type>();
                    if(state2 != NULL){
                        ret = state2->event(ev);
                        if(state2->is_finish()){
                            _transition<typename event::state2_type, typename event::state2_type::next_type>();
                        }
                    }
                }
            }
        }
        return ret;
    }
private:
    // 添加状态列表中的状态
    template<typename type>
    void _alloc_state(void)
    {
        if(!state_type::is_equal<type, state_nil>()){
            type* the_state = new type();
            the_state->owner_ = this;
            if(state_list_.empty()){
                the_state->super_ = NULL;
            }else{
                the_state->super_ = dynamic_cast<typename type::super_type*>(state_list_.back());
            }
            the_state->enter();
            state_list_.push_back(the_state);
            if(the_state->is_finish()){
                _transition<type, typename type::next_type>();
            }else{
                if(!state_type::is_equal<typename type::entry_type, state_nil>()){
                    _alloc_state<typename type::entry_type>();
                }
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
            ctr_bool is_finish = the_state->is_finish();
            if(is_finish){
                the_state->finished();
            }
            delete the_state;
            if(!is_finish && !is_stop){
                _delete_state<type>();
            }
        }
    }
    // 删除状态列表中的状态
    template<typename type1, typename type2>
    void _transition(void)
    {
        if(state_type::is_equal<typename type1::super_type, typename type2::super_type>()){
            _delete_state<type1>();
            _alloc_state<type2>();
        }
    }
    // 获取状态列表中的状态
    template<typename type>
    type* _get_state(void)
    {
        type* ret = NULL;
        for(auto itor = state_list_.rbegin(); itor != state_list_.rend(); ++itor){
            if(state_type::is_equal<type>(**itor)){
                ret = dynamic_cast<type*>(*itor);
                break;
            }
        }
        return ret;
    }
    
    // 状态列表
    std::list<state_base*> state_list_;
};

template<typename self, typename super>
void state<self, super>::finished()
{
    if(owner_ != NULL){
        owner_->transition<self, typename self::next_type>();
    }
}

CTR_NAMESPACE_END

#endif // _CTR_STATE_MACHINE_HPP_
