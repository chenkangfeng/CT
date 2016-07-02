//
//  ctr_macro.h 2016-07-02
//  宏定义
//

#ifndef _CTR_MACRO_H_
#define _CTR_MACRO_H_

#define CTR_NUM_RSEQ()         \
49,48,47,46,45,44,43,42,41,40, \
39,38,37,36,35,34,33,32,31,30, \
29,28,27,26,25,24,23,22,21,20, \
19,18,17,16,15,14,13,12,11,10, \
 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define CTR_NUM_SEQ(                     \
_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
_41,_42,_43,_44,_45,_46,_47,_48,_49,     \
N,...) N
#define CTR_RSEQ_IN_SEQ(...) CTR_NUM_SEQ(__VA_ARGS__)
// 获取参数个数
#define CTR_ARGS_NUM(...) CTR_RSEQ_IN_SEQ(__VA_ARGS__, CTR_NUM_RSEQ())

#define CTR_NAMESPACE_BEGIN namespace ctr{
#define CTR_NAMESPACE_END   }
#define CTR_USING_NAMESPACE using namespace ctr

#endif // _CTR_MACRO_H_
