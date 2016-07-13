//
//  ctr_state.hpp 16-07-13
//  状态
//

#ifndef _CTR_STATE_HPP_
#define _CTR_STATE_HPP_

#include "ctr_macro.h"
#include "ctr_type.h"
#include <typeinfo>
#include <functional>

CTR_NAMESPACE_BEGIN

class state_nil;
class state_event;
class state_machine;

#define state_next(state) \
public:                   \
typedef state next_type

#define state_entry(state) \
public:                    \
typedef state entry_type

// 状态基类
class state_base
{
    friend class state_machine;
public:
    state_base(void):owner_(NULL){}
    virtual ~state_base(void){}
    
    // 顺序状态
    state_next(state_nil);
    // 子状态
    state_entry(state_nil);
    
    // 进入状态
    virtual void enter(void){}
    // 退出状态
    virtual void exit(void){}
    // 更新状态
    virtual void update(ctr_float32){}
    // 状态事件
    ctr_bool event(state_event&){return true;}
    // 状态是否结束
    virtual ctr_bool is_finish(void){return false;}
protected:
    // 执行结束
    virtual void finished(void){}
    
    // 执行的状态机
    state_machine* owner_;
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

// 状态类
template<typename self, typename super = state_nil>
class state : public state_base
{
    friend class state_machine;
public:
    typedef super super_type;
    
    typedef std::function<void(self&)> enter_cb_func;
    typedef std::function<void(self&)> exit_cb_func;
    typedef std::function<void(self&, ctr_float32)> update_cb_func;
    
    state(void):is_finish_(false), super_(NULL){}
    virtual ~state(void){}

    // 进入状态
    virtual void enter(void)
    {
        if(enter_cb){
            enter_cb(*dynamic_cast<self*>(this));
        }
    }
    // 退出状态
    virtual void exit(void)
    {
        if(exit_cb){
            exit_cb(*dynamic_cast<self*>(this));
        }
    }
    // 更新状态
    virtual void update(ctr_float32 dt)
    {
        if(update_cb){
            update_cb(*dynamic_cast<self*>(this), dt);
        }
    }
    // 状态是否结束
    virtual ctr_bool is_finish(void){return is_finish_;}
    // 结束状态
    void finish(void){is_finish_ = true;}
    
    // 获取状态指针
    template<typename type>
    type* state_ptr(void)
    {
        type* ret = NULL;
        if(state_type::is_equal<type>(*this)){
            ret = dynamic_cast<type*>(this);
        }else{
            ret = super_->template state_ptr<type>();
        }
        return ret;
    }
    
    // 进入状态函数
    static enter_cb_func enter_cb;
    // 退出状态函数
    static exit_cb_func exit_cb;
    // 更新状态函数
    static update_cb_func update_cb;
private:
    // 执行结束
    virtual void finished(void);
    // 结束状态条件
    ctr_bool is_finish_;
    // 父状态指针
    super* super_;
};

template<typename self, typename super>
typename state<self, super>::enter_cb_func state<self, super>::enter_cb = enter_cb_func();
template<typename self, typename super>
typename state<self, super>::exit_cb_func state<self, super>::exit_cb = exit_cb_func();
template<typename self, typename super>
typename state<self, super>::update_cb_func state<self, super>::update_cb = update_cb_func();

// 空状态
class state_nil : public state<state_nil>{};

CTR_NAMESPACE_END

#endif // _CTR_STATE_HPP_
