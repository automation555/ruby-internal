#ifndef insns_info__h_
#define insns_info__h_

#include "ruby.h"

#ifdef RUBY_VM

#include "vm_core.h"

#ifdef HAVE_VM_H
#include "vm.h"
#endif

/** -*-c-*-
  This file contains YARV instructions list.

  ----
  This file is auto generated by insns2vm.rb
  DO NOT TOUCH!

  If you want to fix something, you must edit 'template/insns.inc.tmpl'
  or insns2vm.rb
 */


/* BIN : Basic Instruction Name */
#define BIN(n) YARVINSN_##n

enum ruby_vminsn_type {
  BIN(nop)                       = 0,

  BIN(getlocal)                  = 1,

  BIN(setlocal)                  = 2,

  BIN(getspecial)                = 3,

  BIN(setspecial)                = 4,

  BIN(getdynamic)                = 5,

  BIN(setdynamic)                = 6,

  BIN(getinstancevariable)       = 7,

  BIN(setinstancevariable)       = 8,

  BIN(getclassvariable)          = 9,

  BIN(setclassvariable)          = 10,

  BIN(getconstant)               = 11,

  BIN(setconstant)               = 12,

  BIN(getglobal)                 = 13,

  BIN(setglobal)                 = 14,

  BIN(putnil)                    = 15,

  BIN(putself)                   = 16,

  BIN(putobject)                 = 17,

  BIN(putspecialobject)          = 18,

  BIN(putiseq)                   = 19,

  BIN(putstring)                 = 20,

  BIN(concatstrings)             = 21,

  BIN(tostring)                  = 22,

  BIN(toregexp)                  = 23,

  BIN(newarray)                  = 24,

  BIN(duparray)                  = 25,

  BIN(expandarray)               = 26,

  BIN(concatarray)               = 27,

  BIN(splatarray)                = 28,

  BIN(checkincludearray)         = 29,

  BIN(newhash)                   = 30,

  BIN(newrange)                  = 31,

  BIN(pop)                       = 32,

  BIN(dup)                       = 33,

  BIN(dupn)                      = 34,

  BIN(swap)                      = 35,

  BIN(reput)                     = 36,

  BIN(topn)                      = 37,

  BIN(setn)                      = 38,

  BIN(adjuststack)               = 39,

  BIN(defined)                   = 40,

  BIN(trace)                     = 41,

  BIN(defineclass)               = 42,

  BIN(send)                      = 43,

  BIN(invokesuper)               = 44,

  BIN(invokeblock)               = 45,

  BIN(leave)                     = 46,

  BIN(finish)                    = 47,

  BIN(throw)                     = 48,

  BIN(jump)                      = 49,

  BIN(branchif)                  = 50,

  BIN(branchunless)              = 51,

  BIN(getinlinecache)            = 52,

  BIN(onceinlinecache)           = 53,

  BIN(setinlinecache)            = 54,

  BIN(opt_case_dispatch)         = 55,

  BIN(opt_checkenv)              = 56,

  BIN(opt_plus)                  = 57,

  BIN(opt_minus)                 = 58,

  BIN(opt_mult)                  = 59,

  BIN(opt_div)                   = 60,

  BIN(opt_mod)                   = 61,

  BIN(opt_eq)                    = 62,

  BIN(opt_neq)                   = 63,

  BIN(opt_lt)                    = 64,

  BIN(opt_le)                    = 65,

  BIN(opt_gt)                    = 66,

  BIN(opt_ge)                    = 67,

  BIN(opt_ltlt)                  = 68,

  BIN(opt_aref)                  = 69,

  BIN(opt_aset)                  = 70,

  BIN(opt_length)                = 71,

  BIN(opt_size)                  = 72,

  BIN(opt_succ)                  = 73,

  BIN(opt_not)                   = 74,

  BIN(opt_regexpmatch1)          = 75,

  BIN(opt_regexpmatch2)          = 76,

  BIN(opt_call_c_function)       = 77,

  BIN(bitblt)                    = 78,

  BIN(answer)                    = 79,

  VM_INSTRUCTION_SIZE            = 80
};

/** -*-c-*-
  This file contains instruction information for yarv instruction sequence.

  ----
  This file is auto generated by insns2vm.rb
  DO NOT TOUCH!

  If you want to fix something, you must edit 'template/insns_info.inc.tmpl'
  or insns2vm.rb
 */

#define TS_VALUE 'V'
#define TS_GENTRY 'G'
#define TS_DINDEX 'D'
#define TS_VARIABLE '.'
#define TS_CDHASH 'H'
#define TS_IC 'C'
#define TS_ID 'I'
#define TS_LINDEX 'L'
#define TS_FUNCPTR 'F'
#define TS_ISEQ 'S'
#define TS_NUM 'N'
#define TS_OFFSET 'O'

static const char *const insn_name_info[] = {
  "nop",
  "getlocal",
  "setlocal",
  "getspecial",
  "setspecial",
  "getdynamic",
  "setdynamic",
  "getinstancevariable",
  "setinstancevariable",
  "getclassvariable",
  "setclassvariable",
  "getconstant",
  "setconstant",
  "getglobal",
  "setglobal",
  "putnil",
  "putself",
  "putobject",
  "putspecialobject",
  "putiseq",
  "putstring",
  "concatstrings",
  "tostring",
  "toregexp",
  "newarray",
  "duparray",
  "expandarray",
  "concatarray",
  "splatarray",
  "checkincludearray",
  "newhash",
  "newrange",
  "pop",
  "dup",
  "dupn",
  "swap",
  "reput",
  "topn",
  "setn",
  "adjuststack",
  "defined",
  "trace",
  "defineclass",
  "send",
  "invokesuper",
  "invokeblock",
  "leave",
  "finish",
  "throw",
  "jump",
  "branchif",
  "branchunless",
  "getinlinecache",
  "onceinlinecache",
  "setinlinecache",
  "opt_case_dispatch",
  "opt_checkenv",
  "opt_plus",
  "opt_minus",
  "opt_mult",
  "opt_div",
  "opt_mod",
  "opt_eq",
  "opt_neq",
  "opt_lt",
  "opt_le",
  "opt_gt",
  "opt_ge",
  "opt_ltlt",
  "opt_aref",
  "opt_aset",
  "opt_length",
  "opt_size",
  "opt_succ",
  "opt_not",
  "opt_regexpmatch1",
  "opt_regexpmatch2",
  "opt_call_c_function",
  "bitblt",
  "answer",

};

static const char *const insn_operand_info[] = {
  "", 
  "L", 
  "L", 
  "VN", 
  "V", 
  "DN", 
  "DN", 
  "IC", 
  "IC", 
  "I", 
  "I", 
  "I", 
  "I", 
  "G", 
  "G", 
  "", 
  "", 
  "V", 
  "N", 
  "S", 
  "V", 
  "N", 
  "", 
  "NN", 
  "N", 
  "V", 
  "NN", 
  "", 
  "V", 
  "V", 
  "N", 
  "N", 
  "", 
  "", 
  "N", 
  "", 
  "", 
  "N", 
  "N", 
  "N", 
  "NVV", 
  "N", 
  "ISN", 
  "INSNC", 
  "NSN", 
  "NN", 
  "", 
  "", 
  "N", 
  "O", 
  "O", 
  "O", 
  "OC", 
  "OC", 
  "C", 
  "HO", 
  "", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "CC", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "C", 
  "V", 
  "", 
  "F", 
  "", 
  "", 

};

static const int insn_len_info[] = {
  1,
  2,
  2,
  3,
  2,
  3,
  3,
  3,
  3,
  2,
  2,
  2,
  2,
  2,
  2,
  1,
  1,
  2,
  2,
  2,
  2,
  2,
  1,
  3,
  2,
  2,
  3,
  1,
  2,
  2,
  2,
  2,
  1,
  1,
  2,
  1,
  1,
  2,
  2,
  2,
  4,
  2,
  4,
  6,
  4,
  3,
  1,
  1,
  2,
  2,
  2,
  2,
  3,
  3,
  2,
  3,
  1,
  2,
  2,
  2,
  2,
  2,
  2,
  3,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  2,
  1,
  2,
  1,
  1,

};

#ifdef USE_INSN_RET_NUM
static const int insn_stack_push_num_info[] = {
  0,
  1,
  0,
  1,
  0,
  1,
  0,
  1,
  0,
  1,
  0,
  1,
  0,
  1,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  2,
  1,
  1,
  0,
  2,
  1,
  2,
  1,
  1,
  1,
  1,
  1,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  0,
  0,
  0,
  1,
  1,
  1,
  0,
  0,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  0,
  1,
  1,

};
#endif

#ifdef USE_INSN_STACK_INCREASE
static int
insn_stack_increase(int depth, int insn, VALUE *opes)
{
  switch(insn){
        case BIN(nop):{
          return depth + 0;
        }
        case BIN(getlocal):{
          return depth + 1;
        }
        case BIN(setlocal):{
          return depth + -1;
        }
        case BIN(getspecial):{
          return depth + 1;
        }
        case BIN(setspecial):{
          return depth + -1;
        }
        case BIN(getdynamic):{
          return depth + 1;
        }
        case BIN(setdynamic):{
          return depth + -1;
        }
        case BIN(getinstancevariable):{
          return depth + 1;
        }
        case BIN(setinstancevariable):{
          return depth + -1;
        }
        case BIN(getclassvariable):{
          return depth + 1;
        }
        case BIN(setclassvariable):{
          return depth + -1;
        }
        case BIN(getconstant):{
          return depth + 0;
        }
        case BIN(setconstant):{
          return depth + -2;
        }
        case BIN(getglobal):{
          return depth + 1;
        }
        case BIN(setglobal):{
          return depth + -1;
        }
        case BIN(putnil):{
          return depth + 1;
        }
        case BIN(putself):{
          return depth + 1;
        }
        case BIN(putobject):{
          return depth + 1;
        }
        case BIN(putspecialobject):{
          return depth + 1;
        }
        case BIN(putiseq):{
          return depth + 1;
        }
        case BIN(putstring):{
          return depth + 1;
        }
        case BIN(concatstrings):{
          int inc = 0;
        int num = FIX2INT(opes[0]);
        inc += 1 - num;;
        return depth + inc;
        }
        case BIN(tostring):{
          return depth + 0;
        }
        case BIN(toregexp):{
          int inc = 0;
        int cnt = FIX2INT(opes[1]);
        inc += 1 - cnt;;
        return depth + inc;
        }
        case BIN(newarray):{
          int inc = 0;
        int num = FIX2INT(opes[0]);
        inc += 1 - num;;
        return depth + inc;
        }
        case BIN(duparray):{
          return depth + 1;
        }
        case BIN(expandarray):{
          int inc = 0;
        int num = FIX2INT(opes[0]);
        int flag = FIX2INT(opes[1]);
        inc += num - 1 + (flag & 1 ? 1 : 0);;
        return depth + inc;
        }
        case BIN(concatarray):{
          return depth + -1;
        }
        case BIN(splatarray):{
          return depth + 0;
        }
        case BIN(checkincludearray):{
          return depth + 0;
        }
        case BIN(newhash):{
          int inc = 0;
        int num = FIX2INT(opes[0]);
        inc += 1 - num;;
        return depth + inc;
        }
        case BIN(newrange):{
          return depth + -1;
        }
        case BIN(pop):{
          return depth + -1;
        }
        case BIN(dup):{
          return depth + 1;
        }
        case BIN(dupn):{
          int inc = 0;
        int n = FIX2INT(opes[0]);
        inc += n;;
        return depth + inc;
        }
        case BIN(swap):{
          return depth + 0;
        }
        case BIN(reput):{
          int inc = 0;
        inc += 0;;
        return depth + inc;
        }
        case BIN(topn):{
          int inc = 0;
        inc += 1;;
        return depth + inc;
        }
        case BIN(setn):{
          int inc = 0;
        inc += 0;
        return depth + inc;
        }
        case BIN(adjuststack):{
          int inc = 0;
        int n = FIX2INT(opes[0]);
        inc -= n;
        return depth + inc;
        }
        case BIN(defined):{
          return depth + 0;
        }
        case BIN(trace):{
          return depth + 0;
        }
        case BIN(defineclass):{
          return depth + -1;
        }
        case BIN(send):{
          int inc = 0;
        int op_argc = FIX2INT(opes[1]);
        int op_flag = FIX2INT(opes[3]);
        inc += - (int)(op_argc + ((op_flag & VM_CALL_ARGS_BLOCKARG_BIT) ? 1 : 0));;
        return depth + inc;
        }
        case BIN(invokesuper):{
          int inc = 0;
        int op_argc = FIX2INT(opes[0]);
        int op_flag = FIX2INT(opes[2]);
        inc += - (int)(op_argc + ((op_flag & VM_CALL_ARGS_BLOCKARG_BIT) ? 1 : 0));;
        return depth + inc;
        }
        case BIN(invokeblock):{
          int inc = 0;
        int num = FIX2INT(opes[0]);
        inc += 1 - num;;
        return depth + inc;
        }
        case BIN(leave):{
          return depth + 0;
        }
        case BIN(finish):{
          return depth + 0;
        }
        case BIN(throw):{
          return depth + 0;
        }
        case BIN(jump):{
          return depth + 0;
        }
        case BIN(branchif):{
          return depth + -1;
        }
        case BIN(branchunless):{
          return depth + -1;
        }
        case BIN(getinlinecache):{
          return depth + 1;
        }
        case BIN(onceinlinecache):{
          return depth + 1;
        }
        case BIN(setinlinecache):{
          return depth + 0;
        }
        case BIN(opt_case_dispatch):{
          int inc = 0;
        inc += -1;;
        return depth + inc;
        }
        case BIN(opt_checkenv):{
          return depth + 0;
        }
        case BIN(opt_plus):{
          return depth + -1;
        }
        case BIN(opt_minus):{
          return depth + -1;
        }
        case BIN(opt_mult):{
          return depth + -1;
        }
        case BIN(opt_div):{
          return depth + -1;
        }
        case BIN(opt_mod):{
          return depth + -1;
        }
        case BIN(opt_eq):{
          return depth + -1;
        }
        case BIN(opt_neq):{
          return depth + -1;
        }
        case BIN(opt_lt):{
          return depth + -1;
        }
        case BIN(opt_le):{
          return depth + -1;
        }
        case BIN(opt_gt):{
          return depth + -1;
        }
        case BIN(opt_ge):{
          return depth + -1;
        }
        case BIN(opt_ltlt):{
          return depth + -1;
        }
        case BIN(opt_aref):{
          return depth + -1;
        }
        case BIN(opt_aset):{
          return depth + -2;
        }
        case BIN(opt_length):{
          return depth + 0;
        }
        case BIN(opt_size):{
          return depth + 0;
        }
        case BIN(opt_succ):{
          return depth + 0;
        }
        case BIN(opt_not):{
          return depth + 0;
        }
        case BIN(opt_regexpmatch1):{
          return depth + 0;
        }
        case BIN(opt_regexpmatch2):{
          return depth + -1;
        }
        case BIN(opt_call_c_function):{
          return depth + 0;
        }
        case BIN(bitblt):{
          return depth + 1;
        }
        case BIN(answer):{
          return depth + 1;
        }

  default:
    rb_bug("insn_sp_increase: unreachable");
  }
  return 0;
}
#endif

/* some utilities */

static int
insn_len(VALUE insn)
{
  return insn_len_info[(int)insn];
}

static const char *
insn_name(VALUE insn)
{
  return insn_name_info[(int)insn];
}

static const char *
insn_op_types(VALUE insn)
{
  return insn_operand_info[(int)insn];
}

static int
insn_op_type(VALUE insn, long pos)
{
  int len = insn_len(insn) - 1;
  if(pos < len){
    return insn_operand_info[(int)insn][pos];
  }
  else{
    return 0;
  }
}

#ifdef USE_INSN_RET_NUM
static int
insn_ret_num(VALUE insn)
{
  return insn_stack_push_num_info[(int)insn];
}
#endif
void
make_compile_option(rb_compile_option_t *option, VALUE opt);
void
make_compile_option_value(rb_compile_option_t *option);

extern VALUE instruction_class[VM_INSTRUCTION_SIZE];
void define_instruction_subclasses(VALUE rb_cInstruction);

#endif

#endif

