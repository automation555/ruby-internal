#include <ruby.h>

#ifdef RUBY_VM

/* call-seq:
 *   iseq.self => VM::InstructionSequence
 *
 * Returns the self member of the instruction sequence.
 */
static VALUE iseq_self(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return iseqdat->self;
}

/* call-seq:
 *   iseq.name => String
 *
 * Returns the name of the instruction sequence.
 */
static VALUE iseq_name(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return iseqdat->name;
}

/* call-seq:
 *   iseq.filename => String
 *
 * Returns the filename of the instruction sequence.
 */
static VALUE iseq_filename(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return iseqdat->filename;
}

/* call-seq:
 *   iseq.local_table => String
 *
 * Returns the sequence's local table.
 */
static VALUE iseq_local_table(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  VALUE ary = rb_ary_new();
  int j;

  for(j = 0; j < iseqdat->local_table_size; ++j)
  {
    ID id = iseqdat->local_table[j];
    if(rb_id2name(id))
    {
      rb_ary_push(ary, ID2SYM(id));
    }
    else
    {
      // Temporary
      rb_ary_push(ary, Qnil);
    }
  }

  return ary;
}

/* call-seq:
 *   iseq.argc => String
 *
 * Returns the number of non-optional arguments.
 */
static VALUE iseq_argc(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return INT2NUM(iseqdat->argc);
}

/* call-seq:
 *   iseq.arg_simple => true or false
 *
 * Returns true if this sequence takes only simple (non-rest, non-opt,
 * non-block) args.
 */
static VALUE iseq_arg_simple(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return iseqdat->arg_simple ? Qtrue : Qfalse;
}

/* call-seq:
 *   iseq.arg_rest => true or false
 *
 * Returns the index of the rest (*x) arg.
 */
static VALUE iseq_arg_rest(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return INT2NUM(iseqdat->arg_rest);
}

/* call-seq:
 *   iseq.arg_block => true or false
 *
 * Returns the index of the block (&x) arg.
 */
static VALUE iseq_arg_block(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  return INT2NUM(iseqdat->arg_block);
}

/* call-seq:
 *   iseq.arg_opt_table => Array of Integer
 *
 * Returns optional argument table.  The value in the table represent
 * the index into the instruction sequence of the code to set the
 * optional argument.  The last element is the index of the start of the
 * code sequence.
 */
static VALUE iseq_arg_opt_table(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  VALUE ary = rb_ary_new();
  int j;

  for(j = 0; j < iseqdat->arg_opts; ++j)
  {
    rb_ary_push(ary, INT2NUM(iseqdat->arg_opt_table[j]));
  }

  return ary;
}

/* call-seq:
 *   iseq.each(&block) => nil
 *
 * Yields each instruction in the sequence.
 */
static VALUE iseq_each(VALUE self)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  VALUE * seq;

  for(seq = iseqdat->iseq; seq < iseqdat->iseq + iseqdat->iseq_size; )
  {
    VALUE insn = *seq++;
    int op_type_idx;
    int len = insn_len(insn);
    VALUE args = rb_ary_new();

    for(op_type_idx = 0; op_type_idx < len-1; ++op_type_idx, ++seq)
    {
      switch(insn_op_type(insn, op_type_idx))
      {
        case TS_VALUE:
          rb_ary_push(args, *seq);
          break;

        case TS_LINDEX:
        case TS_DINDEX:
        case TS_NUM:
          rb_ary_push(args, INT2FIX(*seq));
          break;

        case TS_ISEQ:
        {
          rb_iseq_t * iseq = (rb_iseq_t *)*seq;
          if(iseq)
          {
            rb_ary_push(args, iseq->self);
          }
          else
          {
            rb_ary_push(args, Qnil);
          }
          break;
        }

        case TS_GENTRY:
        {
          struct global_entry *entry = (struct global_entry *)*seq;
          rb_ary_push(args, ID2SYM(rb_intern(rb_id2name(entry->id))));
          break;
        }

        case TS_OFFSET:
          rb_ary_push(args, Qnil);
          /* TODO */
          break;

        case TS_VARIABLE:
          rb_ary_push(args, Qnil);
          /* TODO */
          break;

        case TS_CDHASH:
          rb_ary_push(args, Qnil);
          /* TODO */
          break;

        case TS_IC:
        {
          NODE * ic = (NODE *)*seq;
          rb_ary_push(args, wrap_node_as(ic, rb_cInlineCache));
          break;
        }

        case TS_ID:
          rb_ary_push(args, ID2SYM(*seq));
          break;
      }
    }

    rb_yield(rb_class_new_instance(
            RARRAY(args)->len,
            RARRAY(args)->ptr,
            instruction_class[insn]));
  }

  return Qnil;
}

/* call-seq:
 *   iseq.insn_line(n) => Integer
 *
 * Returns the line number of the nth instruction in the sequence.
 */
static VALUE iseq_insn_line(VALUE self, VALUE n)
{
  rb_iseq_t *iseqdat = iseq_check(self);
  unsigned long pos = NUM2LONG(n);
  unsigned long i, size = iseqdat->insn_info_size;
  struct iseq_insn_info_entry *iiary = iseqdat->insn_info_table;

  for (i = 0; i < size; i++) {
      if (iiary[i].position == pos) {
          return INT2NUM(iiary[i].line_no);
      }
  }
  return Qnil;
}

/* The putobject instruction takes a VALUE as a parameter.  But if this
 * value is a class, we'll end up trying to dump the class!  That's
 * probably not what we want, so we use a placeholder instead.
 */
void convert_modules_to_placeholders(VALUE array)
{
  int j;

  for(j = 0; j < RARRAY(array)->len; ++j)
  {
    VALUE v = RARRAY(array)->ptr[j];
    if(TYPE(v) == T_ARRAY)
    {
      convert_modules_to_placeholders(v);
    }
    else if(TYPE(v) == T_MODULE || TYPE(v) == T_CLASS)
    {
      VALUE p = rb_class_new_instance(0, 0, rb_cModulePlaceholder);
      VALUE sym = rb_mod_name(v);
      rb_iv_set(p, "name", sym);
      RARRAY(array)->ptr[j] = p;
    }
  }
}

void convert_placeholders_to_modules(VALUE array)
{
  int j;

  for(j = 0; j < RARRAY(array)->len; ++j)
  {
    VALUE v = RARRAY(array)->ptr[j];
    if(TYPE(v) == T_ARRAY)
    {
      convert_placeholders_to_modules(v);
    }
    else if(CLASS_OF(v) == rb_cModulePlaceholder)
    {
      VALUE sym = rb_ivar_get(v, rb_intern("name"));
      VALUE klass = 
        rb_funcall(lookup_module_proc, rb_intern("call"), 1, sym);
      RARRAY(array)->ptr[j] = klass;
    }
  }
}

/*
 * call-seq:
 *   iseq.dump(limit) => String
 *
 * Dumps VM::InstuctionSequence to a string (only available on YARV).
 */
static VALUE iseq_marshal_dump(VALUE self, VALUE limit)
{
  VALUE arr;

  if(ruby_safe_level >= 4)
  {
    /* no access to potentially sensitive data from the sandbox */
    rb_raise(rb_eSecurityError, "Insecure: can't dump iseq");
  }

  arr = iseq_data_to_ary((rb_iseq_t *)DATA_PTR(self));
  convert_modules_to_placeholders(arr);

  return marshal_dump(arr, limit);
}

/*
 * call-seq:
 *   VM::InstructionSequence.load(String) => VM::InstructionSequence
 *
 * Load a VM::InstuctionSequence from a string (only available on YARV).
 */
static VALUE iseq_marshal_load(VALUE klass, VALUE str)
{
  VALUE arr;

  if(   ruby_safe_level >= 4
     || (ruby_safe_level >= 1 && OBJ_TAINTED(str)))
  {
    /* no playing with knives in the sandbox */
    rb_raise(rb_eSecurityError, "Insecure: can't load iseq");
  }

  arr = marshal_load(str);
  convert_placeholders_to_modules(arr);

  VALUE iseq = iseq_load(Qnil, arr, 0, Qnil);
  return iseq;
}

void dump_iseq_to_hash(VALUE iseq, VALUE node_hash)
{
  if(RTEST(rb_hash_aref(node_hash, rb_obj_id(iseq))))
  {
    return;
  }

  rb_hash_aset(node_hash, rb_obj_id(iseq), iseq);
}

VALUE load_iseq_from_hash(VALUE iseq, VALUE orig_node_id, VALUE node_hash, VALUE id_hash)
{
  rb_hash_aset(id_hash, orig_node_id, rb_obj_id(iseq));
  return iseq;
}

#endif

void Init_iseq(void)
{
#ifdef RUBY_VM
  /* For rdoc: rb_cVM = rb_define_class("VM", rb_cObject); */
  /* For rdoc: rb_cISeq = rb_define_class_under(rb_cVM, "InstructionSequence", rb_cObject) */
  rb_define_method(rb_cISeq, "self", iseq_self, 0);
  rb_define_method(rb_cISeq, "name", iseq_name, 0);
  rb_define_method(rb_cISeq, "filename", iseq_filename, 0);
  rb_define_method(rb_cISeq, "local_table", iseq_local_table, 0);
  rb_define_method(rb_cISeq, "argc", iseq_argc, 0);
  rb_define_method(rb_cISeq, "arg_simple", iseq_arg_simple, 0);
  rb_define_method(rb_cISeq, "arg_rest", iseq_arg_rest, 0);
  rb_define_method(rb_cISeq, "arg_block", iseq_arg_block, 0);
  rb_define_method(rb_cISeq, "arg_opt_table", iseq_arg_opt_table, 0);
  rb_define_method(rb_cISeq, "each", iseq_each, 0);
  rb_define_method(rb_cISeq, "insn_line", iseq_insn_line, 1);
  rb_include_module(rb_cISeq, rb_mEnumerable);
  rb_define_method(rb_cISeq, "_dump", iseq_marshal_dump, 1);
  rb_define_singleton_method(rb_cISeq, "_load", iseq_marshal_load, 1);
#endif
}

