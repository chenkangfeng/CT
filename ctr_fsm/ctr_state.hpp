//
//  ctr_state.hpp 16-07-02
//  状态
//

#ifndef _CTR_STATE_HPP_
#define _CTR_STATE_HPP_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <typeinfo>
#include <functional>

CTR_NAMESPACE_BEGIN

class state_event;

// 状态基类
class state_base
{
public:
    virtual ~state_base(void){}
    
    // 进入状态
    virtual void enter(void){}
    // 退出状态
    virtual void exit(void){}
    // 更新状态
    virtual void update(ctr_float32){}
    // 状态事件
    ctr_bool event(state_event&){return true;}
};

// 状态类型
class state_type
{
public:
    // 判断状态是否相同
    template<typename type>
    static ctr_bool is_equal(const state_base& state)
    {return typeid(type) == typeid(state);}
    // 判断状态是否相同
    template<typename type1, typename type2>
    static ctr_bool is_equal(void)
    {return typeid(type1) == typeid(type2);}
    // 判断状态是否相同
    static ctr_bool is_equal(const state_base& state1, const state_base& state2)
    {return typeid(state1) == typeid(state2);}
};

class state_nil;

// 状态类
template<typename super = state_nil, typename entry = state_nil>
class state : public state_base
{
    friend class state_machine;
public:
    typedef super super_type;
    typedef entry entry_type;
    
    state(void):super_state_(NULL){}
    virtual ~state(void){}

    // 获取状态指针
    template<typename type>
    type* state_ptr(void)
    {
        type* ret = NULL;
        if(state_type::is_equal<type>(*this)){
            ret = dynamic_cast<type*>(this);
        }else{
            ret = super_state_->template state_ptr<type>();
        }
        return ret;
    }
protected:
    // 父状态指针
    super* super_state_;
};

// 空状态
class state_nil : public state<>{};

// 扩展状态类
template<typename self, typename super = state_nil, typename entry = state_nil>
class state_ex : public state<super, entry>
{
public:
    typedef std::function<void(self&)> enter_func_type;
    typedef std::function<void(self&)> exit_func_type;
    typedef std::function<void(self&, ctr_float32)> update_func_type;
    
    virtual ~state_ex(void){}
    
    // 进入状态
    virtual void enter(void)
    {
        if(enter_func){
            enter_func(*dynamic_cast<self*>(this));
        }
    }
    // 退出状态
    virtual void exit(void)
    {
        if(exit_func){
            exit_func(*dynamic_cast<self*>(this));
        }
    }
    // 更新状态
    virtual void update(ctr_float32 dt)
    {
        if(update_func){
            update_func(*dynamic_cast<self*>(this), dt);
        }
    }
    
    // 进入状态函数
    static enter_func_type enter_func;
    // 退出状态函数
    static exit_func_type exit_func;
    // 更新状态函数
    static update_func_type update_func;
};

template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::enter_func_type state_ex<self, super, entry>::enter_func = enter_func_type();
template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::exit_func_type state_ex<self, super, entry>::exit_func = exit_func_type();
template<typename self, typename super, typename entry>
typename state_ex<self, super, entry>::update_func_type state_ex<self, super, entry>::update_func = update_func_type();

CTR_NAMESPACE_END

#endif // _CTR_STATE_HPP_
