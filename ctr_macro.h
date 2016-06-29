//
//  ctr_macro.h 2016-6-22
//

#ifndef _ctr_macro_h_
#define _ctr_macro_h_

#define ctr_num_rseq()         \
49,48,47,46,45,44,43,42,41,40, \
39,38,37,36,35,34,33,32,31,30, \
29,28,27,26,25,24,23,22,21,20, \
19,18,17,16,15,14,13,12,11,10, \
 9, 8, 7, 6, 5, 4, 3, 2, 1, 0
#define ctr_num_seq(                     \
_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
_11,_12,_13,_14,_15,_16,_17,_18,_19,_20, \
_21,_22,_23,_24,_25,_26,_27,_28,_29,_30, \
_31,_32,_33,_34,_35,_36,_37,_38,_39,_40, \
_41,_42,_43,_44,_45,_46,_47,_48,_49,     \
N,...) N
#define ctr_rseq_in_seq(...) ctr_num_seq(__VA_ARGS__)
#define ctr_args_count(...) ctr_rseq_in_seq(__VA_ARGS__, ctr_num_rseq())

#define ctr_namespace_begin namespace ctr{
#define ctr_namespace_end   }
#define ctr_using_namespace using namespace ctr

#endif // _ctr_macro_h_
