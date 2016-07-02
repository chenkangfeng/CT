//
//  ctr_state_event.hpp 16-07-02
//  状态事件
//

#ifndef _CTR_STATE_EVENT_HPP_
#define _CTR_STATE_EVENT_HPP_

#include "ctr_macro.h"
#include "ctr_type.h"

CTR_NAMESPACE_BEGIN

class state_nil;

// 状态事件基类
class state_event
{
public:
    typedef state_event event_type;
    
    typedef state_nil state1_type;
    typedef state_nil state2_type;
    
    virtual ~state_event(void){}
};

// 刷新状态事件
template<typename type>
class state_refresh : public state_event
{
public:
    typedef state_refresh event_type;
    
    typedef type state1_type;
    typedef type state2_type;
    
    virtual ~state_refresh(void){}
};

// 转换状态事件
template<typename type1, typename type2>
class state_transition : public state_event
{
public:
    typedef state_transition event_type;
    
    typedef type1 state1_type;
    typedef type2 state2_type;
    
    virtual ~state_transition(void){}
};

// 状态事件类型
class state_event_type
{
public:
    template<typename type>
    static ctr_bool is_refresh(void)
    {
        return (typeid(typename type::event_type) ==
        typeid(state_refresh<typename type::state1_type>));
    }
    template<typename type>
    static ctr_bool is_transition(void)
    {
        return (typeid(typename type::event_type) ==
        typeid(state_transition<typename type::state1_type, typename type::state2_type>));
    }
};

CTR_NAMESPACE_END

#endif // _CTR_STATE_EVENT_HPP_
