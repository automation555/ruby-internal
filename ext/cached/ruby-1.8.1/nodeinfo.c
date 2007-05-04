#include "nodeinfo.h"
#include "version.h"
#include "global_entry.h"
#include "builtins.h"

static int is_flip_state(ID id)
{
  if(id == 0)
  {
    return 1;
  }
#if RUBY_VERSION_CODE >= 180
  if(!rb_is_local_id(id))
  {
    return 1;
  }
#endif
  return 0;
}

static VALUE variable_names(ID * tbl)
{
  if(tbl)
  {
    size_t j;
    VALUE arr = rb_ary_new();
    /* A tbl contains the names of local variables.  The first
     * element is the size of the table.  The next two elements
     * are $_ and $~.  The rest of the elements are the names of
     * the variables themselves.
     */
    for(j = 3; j <= tbl[0]; ++j)
    {
      if(is_flip_state(tbl[j]))
      {
        /* flip state */
        rb_ary_push(arr, Qnil);
      }
      else
      {
        rb_ary_push(arr, ID2SYM(tbl[j]));
      }
    }
    return arr;
  }
  else
  {
    return Qnil;
  }
}

/* Every node contains three elements.  This function takes any one of
 * those elements, converts it to a Ruby object that can be dumped
 * (since nodes can't be dumped), and puts the resulting object into
 * node_hash.
 */
VALUE dump_node_elem(enum Node_Elem_Name nen, NODE * n, VALUE node_hash)
{
  switch(nen)
  {
    case NEN_NONE:
      return Qnil;
    case NEN_1ST:
      if(n->nd_1st)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_1st, node_hash);
        return node_id(n->nd_1st);
      }
      else
      {
        return Qnil;
      }
    case NEN_2ND:
      if(n->nd_2nd)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_2nd, node_hash);
        return node_id(n->nd_2nd);
      }
      else
      {
        return Qnil;
      }
    case NEN_AID:
      if(n->nd_aid == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for aid (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_aid);
    case NEN_ALEN:
      {
      return LONG2NUM((long)n->nd_alen);
      }
    case NEN_ARGC:
      {
      return LONG2NUM((long)n->nd_argc);
      }
    case NEN_ARGS:
      if(n->nd_args)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_args, node_hash);
        return node_id(n->nd_args);
      }
      else
      {
        return Qnil;
      }
    case NEN_BEG:
      if(n->nd_beg)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_beg, node_hash);
        return node_id(n->nd_beg);
      }
      else
      {
        return Qnil;
      }
    case NEN_BODY:
      if(n->nd_body)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_body, node_hash);
        return node_id(n->nd_body);
      }
      else
      {
        return Qnil;
      }
    case NEN_CFLAG:
      {
      return LONG2NUM((long)n->nd_cflag);
      }
    case NEN_CFNC:
      rb_raise(rb_eArgError, "Cannot dump cfunc");
    case NEN_CLSS:
      switch(TYPE(n->nd_clss))
      {
        case T_CLASS:
        case T_MODULE:
        {
          /* When dumping a class, we dump just the name (otherwise we'd
           * get multiple copies of the class each time we load a method
           * on the other side).
           */
          VALUE klass = (VALUE)n->nd_clss;
          VALUE path;
          if(FL_TEST(klass, FL_SINGLETON))
          {
            VALUE singleton = rb_iv_get(klass, "__attached__");
            path = rb_class_path(singleton);
          }
          else
          {
            path = rb_class_path(klass);
            if(STR2CSTR(path)[0] == '#')
            {
              rb_raise(rb_eArgError, "cannot dump anonymous class");
            }
          }
          return rb_assoc_new(
              INT2NUM(((struct RBasic *)(n->nd_clss))->flags),
              path);
        }
        
        default:
          /* TODO: would like to dump flags, not type */
          return rb_assoc_new(
              INT2NUM(TYPE((VALUE)n->nd_clss)),
              (VALUE)n->nd_clss);
      }
    case NEN_CNT:
      {
      return LONG2NUM((long)n->nd_cnt);
      }
    case NEN_COND:
      if(n->nd_cond)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_cond, node_hash);
        return node_id(n->nd_cond);
      }
      else
      {
        return Qnil;
      }
    case NEN_CPATH:
      if(n->nd_cpath)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_cpath, node_hash);
        return node_id(n->nd_cpath);
      }
      else
      {
        return Qnil;
      }
    case NEN_CVAL:
      switch(TYPE(n->nd_cval))
      {
        case T_CLASS:
        case T_MODULE:
        {
          /* When dumping a class, we dump just the name (otherwise we'd
           * get multiple copies of the class each time we load a method
           * on the other side).
           */
          VALUE klass = (VALUE)n->nd_cval;
          VALUE path;
          if(FL_TEST(klass, FL_SINGLETON))
          {
            VALUE singleton = rb_iv_get(klass, "__attached__");
            path = rb_class_path(singleton);
          }
          else
          {
            path = rb_class_path(klass);
            if(STR2CSTR(path)[0] == '#')
            {
              rb_raise(rb_eArgError, "cannot dump anonymous class");
            }
          }
          return rb_assoc_new(
              INT2NUM(((struct RBasic *)(n->nd_cval))->flags),
              path);
        }
        
        default:
          /* TODO: would like to dump flags, not type */
          return rb_assoc_new(
              INT2NUM(TYPE((VALUE)n->nd_cval)),
              (VALUE)n->nd_cval);
      }
    case NEN_DEFN:
      if(n->nd_defn)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_defn, node_hash);
        return node_id(n->nd_defn);
      }
      else
      {
        return Qnil;
      }
    case NEN_ELSE:
      if(n->nd_else)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_else, node_hash);
        return node_id(n->nd_else);
      }
      else
      {
        return Qnil;
      }
    case NEN_END:
      if(n->nd_end)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_end, node_hash);
        return node_id(n->nd_end);
      }
      else
      {
        return Qnil;
      }
    case NEN_ENSR:
      if(n->nd_ensr)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_ensr, node_hash);
        return node_id(n->nd_ensr);
      }
      else
      {
        return Qnil;
      }
    case NEN_ENTRY:
      if(n->nd_entry->id == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for entry (node type %d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_entry->id);
    case NEN_FRML:
      if(n->nd_frml)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_frml, node_hash);
        return node_id(n->nd_frml);
      }
      else
      {
        return Qnil;
      }
    case NEN_HEAD:
      if(n->nd_head)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_head, node_hash);
        return node_id(n->nd_head);
      }
      else
      {
        return Qnil;
      }
    case NEN_IBDY:
      if(n->nd_ibdy)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_ibdy, node_hash);
        return node_id(n->nd_ibdy);
      }
      else
      {
        return Qnil;
      }
    case NEN_ITER:
      if(n->nd_iter)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_iter, node_hash);
        return node_id(n->nd_iter);
      }
      else
      {
        return Qnil;
      }
    case NEN_LIT:
      switch(TYPE(n->nd_lit))
      {
        case T_CLASS:
        case T_MODULE:
        {
          /* When dumping a class, we dump just the name (otherwise we'd
           * get multiple copies of the class each time we load a method
           * on the other side).
           */
          VALUE klass = (VALUE)n->nd_lit;
          VALUE path;
          if(FL_TEST(klass, FL_SINGLETON))
          {
            VALUE singleton = rb_iv_get(klass, "__attached__");
            path = rb_class_path(singleton);
          }
          else
          {
            path = rb_class_path(klass);
            if(STR2CSTR(path)[0] == '#')
            {
              rb_raise(rb_eArgError, "cannot dump anonymous class");
            }
          }
          return rb_assoc_new(
              INT2NUM(((struct RBasic *)(n->nd_lit))->flags),
              path);
        }
        
        default:
          /* TODO: would like to dump flags, not type */
          return rb_assoc_new(
              INT2NUM(TYPE((VALUE)n->nd_lit)),
              (VALUE)n->nd_lit);
      }
    case NEN_MID:
      if(n->nd_mid == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for mid (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_mid);
    case NEN_MODL:
      if(n->nd_modl == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for modl (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_modl);
    case NEN_NEW:
      if(n->nd_new == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for new (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_new);
    case NEN_NEXT:
      if(n->nd_next)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_next, node_hash);
        return node_id(n->nd_next);
      }
      else
      {
        return Qnil;
      }
    case NEN_NOEX:
      {
      return LONG2NUM((long)n->nd_noex);
      }
    case NEN_NTH:
      {
      return LONG2NUM((long)n->nd_nth);
      }
    case NEN_OLD:
      if(n->nd_old == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for old (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_old);
    case NEN_OPT:
      if(n->nd_opt)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_opt, node_hash);
        return node_id(n->nd_opt);
      }
      else
      {
        return Qnil;
      }
    case NEN_ORIG:
      switch(TYPE(n->nd_orig))
      {
        case T_CLASS:
        case T_MODULE:
        {
          /* When dumping a class, we dump just the name (otherwise we'd
           * get multiple copies of the class each time we load a method
           * on the other side).
           */
          VALUE klass = (VALUE)n->nd_orig;
          VALUE path;
          if(FL_TEST(klass, FL_SINGLETON))
          {
            VALUE singleton = rb_iv_get(klass, "__attached__");
            path = rb_class_path(singleton);
          }
          else
          {
            path = rb_class_path(klass);
            if(STR2CSTR(path)[0] == '#')
            {
              rb_raise(rb_eArgError, "cannot dump anonymous class");
            }
          }
          return rb_assoc_new(
              INT2NUM(((struct RBasic *)(n->nd_orig))->flags),
              path);
        }
        
        default:
          /* TODO: would like to dump flags, not type */
          return rb_assoc_new(
              INT2NUM(TYPE((VALUE)n->nd_orig)),
              (VALUE)n->nd_orig);
      }
    case NEN_RECV:
      if(n->nd_recv)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_recv, node_hash);
        return node_id(n->nd_recv);
      }
      else
      {
        return Qnil;
      }
    case NEN_RESQ:
      if(n->nd_resq)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_resq, node_hash);
        return node_id(n->nd_resq);
      }
      else
      {
        return Qnil;
      }
    case NEN_REST:
      {
      return LONG2NUM((long)n->nd_rest);
      }
    case NEN_RVAL:
      if(n->nd_2nd)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_2nd, node_hash);
        return node_id(n->nd_2nd);
      }
      else
      {
        return Qnil;
      }
    case NEN_STATE:
      {
      return LONG2NUM((long)n->nd_state);
      }
    case NEN_STTS:
      if(n->nd_stts)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_stts, node_hash);
        return node_id(n->nd_stts);
      }
      else
      {
        return Qnil;
      }
    case NEN_SUPER:
      if(n->nd_super)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_super, node_hash);
        return node_id(n->nd_super);
      }
      else
      {
        return Qnil;
      }
    case NEN_TAG:
      if(n->nd_tag == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for tag (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_tag);
    case NEN_TBL:
      {
      VALUE v = variable_names(n->nd_tbl);
      return v;
      }
    case NEN_TVAL:
      switch(TYPE(n->nd_tval))
      {
        case T_CLASS:
        case T_MODULE:
        {
          /* When dumping a class, we dump just the name (otherwise we'd
           * get multiple copies of the class each time we load a method
           * on the other side).
           */
          VALUE klass = (VALUE)n->nd_tval;
          VALUE path;
          if(FL_TEST(klass, FL_SINGLETON))
          {
            VALUE singleton = rb_iv_get(klass, "__attached__");
            path = rb_class_path(singleton);
          }
          else
          {
            path = rb_class_path(klass);
            if(STR2CSTR(path)[0] == '#')
            {
              rb_raise(rb_eArgError, "cannot dump anonymous class");
            }
          }
          return rb_assoc_new(
              INT2NUM(((struct RBasic *)(n->nd_tval))->flags),
              path);
        }
        
        default:
          /* TODO: would like to dump flags, not type */
          return rb_assoc_new(
              INT2NUM(TYPE((VALUE)n->nd_tval)),
              (VALUE)n->nd_tval);
      }
    case NEN_VALUE:
      if(n->nd_value)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_value, node_hash);
        return node_id(n->nd_value);
      }
      else
      {
        return Qnil;
      }
    case NEN_VAR:
      if(n->nd_var)
      {
        dump_node_or_iseq_to_hash((VALUE)n->nd_var, node_hash);
        return node_id(n->nd_var);
      }
      else
      {
        return Qnil;
      }
    case NEN_VID:
      if(n->nd_vid == 0)
      {
        rb_raise(
            rb_eRuntimeError,
            "Invalid ID for vid (node type=%d)",
            nd_type(n));
      }
      return ID2SYM(n->nd_vid);
  }
  rb_raise(rb_eArgError, "Invalid Node_Elem_Name %d", nen);
}

void load_node_elem(enum Node_Elem_Name nen, VALUE v, NODE * n, VALUE node_hash, VALUE id_hash)
{
  switch(nen)
  {
    case NEN_NONE:
      return;
    case NEN_1ST:
    {
      if(v == Qnil)
      {
        n->nd_1st = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_1st = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_2ND:
    {
      if(v == Qnil)
      {
        n->nd_2nd = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_2nd = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_AID:
    {
      n->nd_aid = SYM2ID(v);
      return;
      }
    case NEN_ALEN:
    {
      n->nd_alen = NUM2LONG(v);
      return;
      }
    case NEN_ARGC:
    {
      n->nd_argc = NUM2LONG(v);
      return;
      }
    case NEN_ARGS:
    {
      if(v == Qnil)
      {
        n->nd_args = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_args = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_BEG:
    {
      if(v == Qnil)
      {
        n->nd_beg = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_beg = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_BODY:
    {
      if(v == Qnil)
      {
        n->nd_body = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_body = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_CFLAG:
    {
      n->nd_cflag = NUM2LONG(v);
      return;
      }
    case NEN_CFNC:
    {
      rb_raise(rb_eRuntimeError, "Cannot load cfunc");
      }
    case NEN_CLSS:
    {
      Check_Type(v, T_ARRAY);
      if(RARRAY_LEN(v) != 2)
      {
        rb_raise(rb_eArgError, "wrong size for array");
      }
      int flags = NUM2INT(RARRAY_PTR(v)[0]);
      switch(flags & T_MASK)
      {
        case T_CLASS:
        case T_MODULE:
        {
          VALUE str = RARRAY_PTR(v)[1];
          Check_Type(str, T_STRING);
          VALUE klass = rb_path2class(STR2CSTR(str));
          if(flags & FL_SINGLETON)
          {
            *(VALUE *)(&n->nd_clss) =
              rb_singleton_class(klass);
          }
          else
          {
            *(VALUE *)(&n->nd_clss) =
              klass;
          }
          break;
        }
        default:
          *(VALUE *)(&n->nd_clss) = RARRAY_PTR(v)[1];
          break;
      }
      return;
      }
    case NEN_CNT:
    {
      n->nd_cnt = NUM2LONG(v);
      return;
      }
    case NEN_COND:
    {
      if(v == Qnil)
      {
        n->nd_cond = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_cond = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_CPATH:
    {
      if(v == Qnil)
      {
        n->nd_cpath = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_cpath = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_CVAL:
    {
      Check_Type(v, T_ARRAY);
      if(RARRAY_LEN(v) != 2)
      {
        rb_raise(rb_eArgError, "wrong size for array");
      }
      int flags = NUM2INT(RARRAY_PTR(v)[0]);
      switch(flags & T_MASK)
      {
        case T_CLASS:
        case T_MODULE:
        {
          VALUE str = RARRAY_PTR(v)[1];
          Check_Type(str, T_STRING);
          VALUE klass = rb_path2class(STR2CSTR(str));
          if(flags & FL_SINGLETON)
          {
            *(VALUE *)(&n->nd_cval) =
              rb_singleton_class(klass);
          }
          else
          {
            *(VALUE *)(&n->nd_cval) =
              klass;
          }
          break;
        }
        default:
          *(VALUE *)(&n->nd_cval) = RARRAY_PTR(v)[1];
          break;
      }
      return;
      }
    case NEN_DEFN:
    {
      if(v == Qnil)
      {
        n->nd_defn = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_defn = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_ELSE:
    {
      if(v == Qnil)
      {
        n->nd_else = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_else = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_END:
    {
      if(v == Qnil)
      {
        n->nd_end = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_end = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_ENSR:
    {
      if(v == Qnil)
      {
        n->nd_ensr = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_ensr = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_ENTRY:
    {
      n->nd_entry = rb_global_entry(SYM2ID(v));
      return;
      }
    case NEN_FRML:
    {
      if(v == Qnil)
      {
        n->nd_frml = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_frml = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_HEAD:
    {
      if(v == Qnil)
      {
        n->nd_head = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_head = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_IBDY:
    {
      if(v == Qnil)
      {
        n->nd_ibdy = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_ibdy = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_ITER:
    {
      if(v == Qnil)
      {
        n->nd_iter = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_iter = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_LIT:
    {
      Check_Type(v, T_ARRAY);
      if(RARRAY_LEN(v) != 2)
      {
        rb_raise(rb_eArgError, "wrong size for array");
      }
      int flags = NUM2INT(RARRAY_PTR(v)[0]);
      switch(flags & T_MASK)
      {
        case T_CLASS:
        case T_MODULE:
        {
          VALUE str = RARRAY_PTR(v)[1];
          Check_Type(str, T_STRING);
          VALUE klass = rb_path2class(STR2CSTR(str));
          if(flags & FL_SINGLETON)
          {
            *(VALUE *)(&n->nd_lit) =
              rb_singleton_class(klass);
          }
          else
          {
            *(VALUE *)(&n->nd_lit) =
              klass;
          }
          break;
        }
        default:
          *(VALUE *)(&n->nd_lit) = RARRAY_PTR(v)[1];
          break;
      }
      return;
      }
    case NEN_MID:
    {
      n->nd_mid = SYM2ID(v);
      return;
      }
    case NEN_MODL:
    {
      n->nd_modl = SYM2ID(v);
      return;
      }
    case NEN_NEW:
    {
      n->nd_new = SYM2ID(v);
      return;
      }
    case NEN_NEXT:
    {
      if(v == Qnil)
      {
        n->nd_next = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_next = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_NOEX:
    {
      n->nd_noex = NUM2LONG(v);
      return;
      }
    case NEN_NTH:
    {
      n->nd_nth = NUM2LONG(v);
      return;
      }
    case NEN_OLD:
    {
      n->nd_old = SYM2ID(v);
      return;
      }
    case NEN_OPT:
    {
      if(v == Qnil)
      {
        n->nd_opt = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_opt = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_ORIG:
    {
      Check_Type(v, T_ARRAY);
      if(RARRAY_LEN(v) != 2)
      {
        rb_raise(rb_eArgError, "wrong size for array");
      }
      int flags = NUM2INT(RARRAY_PTR(v)[0]);
      switch(flags & T_MASK)
      {
        case T_CLASS:
        case T_MODULE:
        {
          VALUE str = RARRAY_PTR(v)[1];
          Check_Type(str, T_STRING);
          VALUE klass = rb_path2class(STR2CSTR(str));
          if(flags & FL_SINGLETON)
          {
            *(VALUE *)(&n->nd_orig) =
              rb_singleton_class(klass);
          }
          else
          {
            *(VALUE *)(&n->nd_orig) =
              klass;
          }
          break;
        }
        default:
          *(VALUE *)(&n->nd_orig) = RARRAY_PTR(v)[1];
          break;
      }
      return;
      }
    case NEN_RECV:
    {
      if(v == Qnil)
      {
        n->nd_recv = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_recv = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_RESQ:
    {
      if(v == Qnil)
      {
        n->nd_resq = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_resq = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_REST:
    {
      n->nd_rest = NUM2LONG(v);
      return;
      }
    case NEN_RVAL:
    {
      if(v == Qnil)
      {
        n->nd_2nd = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_2nd = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_STATE:
    {
      n->nd_state = NUM2LONG(v);
      return;
      }
    case NEN_STTS:
    {
      if(v == Qnil)
      {
        n->nd_stts = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_stts = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_SUPER:
    {
      if(v == Qnil)
      {
        n->nd_super = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_super = id_to_node(nid);
        }
        else
        {
          n->u3.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_TAG:
    {
      n->nd_tag = SYM2ID(v);
      return;
      }
    case NEN_TBL:
    {
      if(v == Qnil)
      {
        n->nd_tbl = 0;
      }
      else
      {
        size_t len, j;
        ID * tmp_tbl;

        Check_Type(v, T_ARRAY);
        len = RARRAY_LEN(v);
        tmp_tbl = ALLOCA_N(ID, len + 3);
        tmp_tbl[0] = (ID)(len + 2);
        tmp_tbl[1] = (ID)('_');
        tmp_tbl[2] = (ID)('~');
        for(j = 0; j < len; ++j)
        {
          VALUE e = RARRAY_PTR(v)[j];
          if(e == Qnil)
          {
            /* flip state */
            tmp_tbl[j + 3] = 0;
          }
          else
          {
            tmp_tbl[j + 3] = SYM2ID(e);
          }
        }
        n->nd_tbl = ALLOC_N(ID, len + 3);
        memcpy(n->nd_tbl, tmp_tbl, sizeof(ID) * (len + 3));
      }
      return;
      }
    case NEN_TVAL:
    {
      Check_Type(v, T_ARRAY);
      if(RARRAY_LEN(v) != 2)
      {
        rb_raise(rb_eArgError, "wrong size for array");
      }
      int flags = NUM2INT(RARRAY_PTR(v)[0]);
      switch(flags & T_MASK)
      {
        case T_CLASS:
        case T_MODULE:
        {
          VALUE str = RARRAY_PTR(v)[1];
          Check_Type(str, T_STRING);
          VALUE klass = rb_path2class(STR2CSTR(str));
          if(flags & FL_SINGLETON)
          {
            *(VALUE *)(&n->nd_tval) =
              rb_singleton_class(klass);
          }
          else
          {
            *(VALUE *)(&n->nd_tval) =
              klass;
          }
          break;
        }
        default:
          *(VALUE *)(&n->nd_tval) = RARRAY_PTR(v)[1];
          break;
      }
      return;
      }
    case NEN_VALUE:
    {
      if(v == Qnil)
      {
        n->nd_value = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_value = id_to_node(nid);
        }
        else
        {
          n->u2.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_VAR:
    {
      if(v == Qnil)
      {
        n->nd_var = 0;
      }
      else
      {
        VALUE nid = rb_hash_aref(id_hash, v);
        if(RTEST(nid))
        {
          n->nd_var = id_to_node(nid);
        }
        else
        {
          n->u1.node = (NODE *)load_node_or_iseq_from_hash(
              v, node_hash, id_hash);
        }
      }
      return;
      }
    case NEN_VID:
    {
      n->nd_vid = SYM2ID(v);
      return;
      }
  }
  rb_raise(rb_eRuntimeError, "Internal error: invalid Node_Elem_Name %d", nen);
}

/*
 * Return the Node's _1st_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_1st(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_1st) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_1st);
  }
  else
  {
    return (VALUE)n->nd_1st;
  }
}
/*
 * Return the Node's _2nd_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_2nd(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_2nd) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_2nd);
  }
  else
  {
    return (VALUE)n->nd_2nd;
  }
}
/*
 * Return the Node's _alen_ member.  The return type is an
 * Integer.
 */
static VALUE node_alen(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_alen);
}
/*
 * Return the Node's _argc_ member.  The return type is an
 * Integer.
 */
static VALUE node_argc(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_argc);
}
/*
 * Return the Node's _args_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_args(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_args) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_args);
  }
  else
  {
    return (VALUE)n->nd_args;
  }
}
/*
 * Return the Node's _beg_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_beg(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_beg) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_beg);
  }
  else
  {
    return (VALUE)n->nd_beg;
  }
}
/*
 * Return the Node's _body_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_body(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_body) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_body);
  }
  else
  {
    return (VALUE)n->nd_body;
  }
}
/*
 * Return the Node's _cflag_ member.  The return type is an
 * Integer.
 */
static VALUE node_cflag(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_cflag);
}
/*
 * Return the Node's _cfnc_ member.  The return type is an
 * integer representation of the cfnc's pointer.
 */
static VALUE node_cfnc(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM((long)(n->nd_cfnc));
}
/*
 * Return the Node's _clss_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_clss(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_clss) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_clss);
  }
  else
  {
    return (VALUE)n->nd_clss;
  }
}
/*
 * Return the Node's _cnt_ member.  The return type is an
 * Integer.
 */
static VALUE node_cnt(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_cnt);
}
/*
 * Return the Node's _cond_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_cond(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_cond) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_cond);
  }
  else
  {
    return (VALUE)n->nd_cond;
  }
}
/*
 * Return the Node's _cpath_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_cpath(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_cpath) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_cpath);
  }
  else
  {
    return (VALUE)n->nd_cpath;
  }
}
/*
 * Return the Node's _cval_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_cval(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_cval) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_cval);
  }
  else
  {
    return (VALUE)n->nd_cval;
  }
}
/*
 * Return the Node's _defn_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_defn(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_defn) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_defn);
  }
  else
  {
    return (VALUE)n->nd_defn;
  }
}
/*
 * Return the Node's _else_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_else(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_else) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_else);
  }
  else
  {
    return (VALUE)n->nd_else;
  }
}
/*
 * Return the Node's _end_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_end(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_end) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_end);
  }
  else
  {
    return (VALUE)n->nd_end;
  }
}
/*
 * Return the Node's _ensr_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_ensr(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_ensr) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_ensr);
  }
  else
  {
    return (VALUE)n->nd_ensr;
  }
}
/*
 * Return the Node's _entry_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_entry(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  if(n->nd_entry->id == 0)
  {
    rb_raise(
        rb_eRuntimeError,
        "Invalid ID for entry (node type %d)",
        nd_type(n));
  }
  return ID2SYM(n->nd_entry->id);
}
/*
 * Return the Node's _head_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_head(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_head) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_head);
  }
  else
  {
    return (VALUE)n->nd_head;
  }
}
/*
 * Return the Node's _iter_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_iter(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_iter) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_iter);
  }
  else
  {
    return (VALUE)n->nd_iter;
  }
}
/*
 * Return the Node's _lit_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_lit(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_lit) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_lit);
  }
  else
  {
    return (VALUE)n->nd_lit;
  }
}
/*
 * Return the Node's _mid_ member.  The return type is a
 * Symbol.
 */
static VALUE node_mid(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  if(n->nd_mid == 0)
  {
    rb_raise(
        rb_eRuntimeError,
        "Invalid ID for mid (node type=%d)",
        nd_type(n));
  }
  return ID2SYM(n->nd_mid);
}
/*
 * Return the Node's _next_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_next(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_next) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_next);
  }
  else
  {
    return (VALUE)n->nd_next;
  }
}
/*
 * Return the Node's _noex_ member.  The return type is an
 * Integer.
 */
static VALUE node_noex(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_noex);
}
/*
 * Return the Node's _nth_ member.  The return type is an
 * Integer.
 */
static VALUE node_nth(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_nth);
}
/*
 * Return the Node's _opt_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_opt(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_opt) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_opt);
  }
  else
  {
    return (VALUE)n->nd_opt;
  }
}
/*
 * Return the Node's _orig_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_orig(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_orig) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_orig);
  }
  else
  {
    return (VALUE)n->nd_orig;
  }
}
/*
 * Return the Node's _recv_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_recv(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_recv) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_recv);
  }
  else
  {
    return (VALUE)n->nd_recv;
  }
}
/*
 * Return the Node's _resq_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_resq(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_resq) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_resq);
  }
  else
  {
    return (VALUE)n->nd_resq;
  }
}
/*
 * Return the Node's _rest_ member.  The return type is an
 * Integer.
 */
static VALUE node_rest(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_rest);
}
/*
 * Return the Node's _rval_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_rval(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_rval) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_rval);
  }
  else
  {
    return (VALUE)n->nd_rval;
  }
}
/*
 * Return the Node's _state_ member.  The return type is an
 * Integer.
 */
static VALUE node_state(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return LONG2NUM(n->nd_state);
}
/*
 * Return the Node's _stts_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_stts(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_stts) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_stts);
  }
  else
  {
    return (VALUE)n->nd_stts;
  }
}
/*
 * Return the Node's _super_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_super(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_super) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_super);
  }
  else
  {
    return (VALUE)n->nd_super;
  }
}
/*
 * Return the Node's _tbl_ member.  The return value is an
 * Array holding names of variables.
 */
static VALUE node_tbl(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  return variable_names(n->nd_tbl);
}
/*
 * Return the Node's _tval_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_tval(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_tval) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_tval);
  }
  else
  {
    return (VALUE)n->nd_tval;
  }
}
/*
 * Return the Node's _value_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_value(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_value) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_value);
  }
  else
  {
    return (VALUE)n->nd_value;
  }
}
/*
 * Return the Node's _var_ member.  The return type is
 * either a Node or an Object.
 */
static VALUE node_var(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);

  if(TYPE(n->nd_var) == T_NODE)
  {
    return wrap_node((NODE *)n->nd_var);
  }
  else
  {
    return (VALUE)n->nd_var;
  }
}
/*
 * Return the Node's _vid_ member.  The return type is a
 * Symbol.
 */
static VALUE node_vid(VALUE self)
{
  NODE * n;
  Data_Get_Struct(self, NODE, n);
  if(n->nd_vid == 0)
  {
    rb_raise(
        rb_eRuntimeError,
        "Invalid ID for vid (node type=%d)",
        nd_type(n));
  }
  return ID2SYM(n->nd_vid);
}

/*
 * Return an array of strings containing the names of the node class's
 * members.
 */
VALUE node_s_members(VALUE klass)
{
  return rb_iv_get(klass, "__member__");
}

void define_node_subclass_methods()
{
  /* For rdoc: rb_cNode = rb_define_class("Node", rb_cObject); */
  VALUE rb_cNode = rb_const_get(rb_cObject, rb_intern("Node"));
  VALUE members;

  {
    VALUE rb_cMETHOD = rb_define_class_under(rb_cNode, "METHOD", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_METHOD] = rb_cMETHOD;
    rb_iv_set(rb_cMETHOD, "__member__", members);
    rb_define_singleton_method(rb_cMETHOD, "members", node_s_members, 0);
    rb_define_method(rb_cMETHOD, "noex", node_noex, 0);
    rb_ary_push(members, rb_str_new2("noex"));
    rb_define_method(rb_cMETHOD, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cMETHOD, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
  {
    VALUE rb_cBLOCK = rb_define_class_under(rb_cNode, "BLOCK", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BLOCK] = rb_cBLOCK;
    rb_iv_set(rb_cBLOCK, "__member__", members);
    rb_define_singleton_method(rb_cBLOCK, "members", node_s_members, 0);
    rb_define_method(rb_cBLOCK, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cBLOCK, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cPOSTEXE = rb_define_class_under(rb_cNode, "POSTEXE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_POSTEXE] = rb_cPOSTEXE;
    rb_iv_set(rb_cPOSTEXE, "__member__", members);
    rb_define_singleton_method(rb_cPOSTEXE, "members", node_s_members, 0);
  }
  {
    VALUE rb_cBEGIN = rb_define_class_under(rb_cNode, "BEGIN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BEGIN] = rb_cBEGIN;
    rb_iv_set(rb_cBEGIN, "__member__", members);
    rb_define_singleton_method(rb_cBEGIN, "members", node_s_members, 0);
    rb_define_method(rb_cBEGIN, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cMATCH = rb_define_class_under(rb_cNode, "MATCH", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MATCH] = rb_cMATCH;
    rb_iv_set(rb_cMATCH, "__member__", members);
    rb_define_singleton_method(rb_cMATCH, "members", node_s_members, 0);
    rb_define_method(rb_cMATCH, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
  }
  {
    VALUE rb_cMATCH2 = rb_define_class_under(rb_cNode, "MATCH2", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MATCH2] = rb_cMATCH2;
    rb_iv_set(rb_cMATCH2, "__member__", members);
    rb_define_singleton_method(rb_cMATCH2, "members", node_s_members, 0);
    rb_define_method(rb_cMATCH2, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cMATCH2, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cMATCH3 = rb_define_class_under(rb_cNode, "MATCH3", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MATCH3] = rb_cMATCH3;
    rb_iv_set(rb_cMATCH3, "__member__", members);
    rb_define_singleton_method(rb_cMATCH3, "members", node_s_members, 0);
    rb_define_method(rb_cMATCH3, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cMATCH3, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cOPT_N = rb_define_class_under(rb_cNode, "OPT_N", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OPT_N] = rb_cOPT_N;
    rb_iv_set(rb_cOPT_N, "__member__", members);
    rb_define_singleton_method(rb_cOPT_N, "members", node_s_members, 0);
    rb_define_method(rb_cOPT_N, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cSELF = rb_define_class_under(rb_cNode, "SELF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_SELF] = rb_cSELF;
    rb_iv_set(rb_cSELF, "__member__", members);
    rb_define_singleton_method(rb_cSELF, "members", node_s_members, 0);
  }
  {
    VALUE rb_cNILNODE = rb_define_class_under(rb_cNode, "NILNODE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_NIL] = rb_cNILNODE;
    rb_iv_set(rb_cNILNODE, "__member__", members);
    rb_define_singleton_method(rb_cNILNODE, "members", node_s_members, 0);
  }
  {
    VALUE rb_cTRUENODE = rb_define_class_under(rb_cNode, "TRUENODE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_TRUE] = rb_cTRUENODE;
    rb_iv_set(rb_cTRUENODE, "__member__", members);
    rb_define_singleton_method(rb_cTRUENODE, "members", node_s_members, 0);
  }
  {
    VALUE rb_cFALSENODE = rb_define_class_under(rb_cNode, "FALSENODE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FALSE] = rb_cFALSENODE;
    rb_iv_set(rb_cFALSENODE, "__member__", members);
    rb_define_singleton_method(rb_cFALSENODE, "members", node_s_members, 0);
  }
  {
    VALUE rb_cIF = rb_define_class_under(rb_cNode, "IF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_IF] = rb_cIF;
    rb_iv_set(rb_cIF, "__member__", members);
    rb_define_singleton_method(rb_cIF, "members", node_s_members, 0);
    rb_define_method(rb_cIF, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cIF, "else", node_else, 0);
    rb_ary_push(members, rb_str_new2("else"));
    rb_define_method(rb_cIF, "cond", node_cond, 0);
    rb_ary_push(members, rb_str_new2("cond"));
  }
  {
    VALUE rb_cWHEN = rb_define_class_under(rb_cNode, "WHEN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_WHEN] = rb_cWHEN;
    rb_iv_set(rb_cWHEN, "__member__", members);
    rb_define_singleton_method(rb_cWHEN, "members", node_s_members, 0);
    rb_define_method(rb_cWHEN, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cWHEN, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cWHEN, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cCASE = rb_define_class_under(rb_cNode, "CASE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CASE] = rb_cCASE;
    rb_iv_set(rb_cCASE, "__member__", members);
    rb_define_singleton_method(rb_cCASE, "members", node_s_members, 0);
    rb_define_method(rb_cCASE, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cCASE, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cCASE, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cWHILE = rb_define_class_under(rb_cNode, "WHILE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_WHILE] = rb_cWHILE;
    rb_iv_set(rb_cWHILE, "__member__", members);
    rb_define_singleton_method(rb_cWHILE, "members", node_s_members, 0);
    rb_define_method(rb_cWHILE, "cond", node_cond, 0);
    rb_ary_push(members, rb_str_new2("cond"));
    rb_define_method(rb_cWHILE, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cWHILE, "state", node_state, 0);
    rb_ary_push(members, rb_str_new2("state"));
  }
  {
    VALUE rb_cUNTIL = rb_define_class_under(rb_cNode, "UNTIL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_UNTIL] = rb_cUNTIL;
    rb_iv_set(rb_cUNTIL, "__member__", members);
    rb_define_singleton_method(rb_cUNTIL, "members", node_s_members, 0);
    rb_define_method(rb_cUNTIL, "cond", node_cond, 0);
    rb_ary_push(members, rb_str_new2("cond"));
    rb_define_method(rb_cUNTIL, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cUNTIL, "state", node_state, 0);
    rb_ary_push(members, rb_str_new2("state"));
  }
  {
    VALUE rb_cBLOCK_PASS = rb_define_class_under(rb_cNode, "BLOCK_PASS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BLOCK_PASS] = rb_cBLOCK_PASS;
    rb_iv_set(rb_cBLOCK_PASS, "__member__", members);
    rb_define_singleton_method(rb_cBLOCK_PASS, "members", node_s_members, 0);
    rb_define_method(rb_cBLOCK_PASS, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cBLOCK_PASS, "iter", node_iter, 0);
    rb_ary_push(members, rb_str_new2("iter"));
  }
  {
    VALUE rb_cITER = rb_define_class_under(rb_cNode, "ITER", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ITER] = rb_cITER;
    rb_iv_set(rb_cITER, "__member__", members);
    rb_define_singleton_method(rb_cITER, "members", node_s_members, 0);
    rb_define_method(rb_cITER, "var", node_var, 0);
    rb_ary_push(members, rb_str_new2("var"));
    rb_define_method(rb_cITER, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cITER, "iter", node_iter, 0);
    rb_ary_push(members, rb_str_new2("iter"));
  }
  {
    VALUE rb_cFOR = rb_define_class_under(rb_cNode, "FOR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FOR] = rb_cFOR;
    rb_iv_set(rb_cFOR, "__member__", members);
    rb_define_singleton_method(rb_cFOR, "members", node_s_members, 0);
    rb_define_method(rb_cFOR, "var", node_var, 0);
    rb_ary_push(members, rb_str_new2("var"));
    rb_define_method(rb_cFOR, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
    rb_define_method(rb_cFOR, "iter", node_iter, 0);
    rb_ary_push(members, rb_str_new2("iter"));
  }
  {
    VALUE rb_cBREAK = rb_define_class_under(rb_cNode, "BREAK", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BREAK] = rb_cBREAK;
    rb_iv_set(rb_cBREAK, "__member__", members);
    rb_define_singleton_method(rb_cBREAK, "members", node_s_members, 0);
    rb_define_method(rb_cBREAK, "stts", node_stts, 0);
    rb_ary_push(members, rb_str_new2("stts"));
  }
  {
    VALUE rb_cNEXT = rb_define_class_under(rb_cNode, "NEXT", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_NEXT] = rb_cNEXT;
    rb_iv_set(rb_cNEXT, "__member__", members);
    rb_define_singleton_method(rb_cNEXT, "members", node_s_members, 0);
    rb_define_method(rb_cNEXT, "stts", node_stts, 0);
    rb_ary_push(members, rb_str_new2("stts"));
  }
  {
    VALUE rb_cREDO = rb_define_class_under(rb_cNode, "REDO", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_REDO] = rb_cREDO;
    rb_iv_set(rb_cREDO, "__member__", members);
    rb_define_singleton_method(rb_cREDO, "members", node_s_members, 0);
  }
  {
    VALUE rb_cRETRY = rb_define_class_under(rb_cNode, "RETRY", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_RETRY] = rb_cRETRY;
    rb_iv_set(rb_cRETRY, "__member__", members);
    rb_define_singleton_method(rb_cRETRY, "members", node_s_members, 0);
  }
  {
    VALUE rb_cYIELD = rb_define_class_under(rb_cNode, "YIELD", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_YIELD] = rb_cYIELD;
    rb_iv_set(rb_cYIELD, "__member__", members);
    rb_define_singleton_method(rb_cYIELD, "members", node_s_members, 0);
    rb_define_method(rb_cYIELD, "stts", node_stts, 0);
    rb_ary_push(members, rb_str_new2("stts"));
  }
  {
    VALUE rb_cRESCUE = rb_define_class_under(rb_cNode, "RESCUE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_RESCUE] = rb_cRESCUE;
    rb_iv_set(rb_cRESCUE, "__member__", members);
    rb_define_singleton_method(rb_cRESCUE, "members", node_s_members, 0);
    rb_define_method(rb_cRESCUE, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cRESCUE, "resq", node_resq, 0);
    rb_ary_push(members, rb_str_new2("resq"));
    rb_define_method(rb_cRESCUE, "else", node_else, 0);
    rb_ary_push(members, rb_str_new2("else"));
  }
  {
    VALUE rb_cRESBODY = rb_define_class_under(rb_cNode, "RESBODY", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_RESBODY] = rb_cRESBODY;
    rb_iv_set(rb_cRESBODY, "__member__", members);
    rb_define_singleton_method(rb_cRESBODY, "members", node_s_members, 0);
    rb_define_method(rb_cRESBODY, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cRESBODY, "resq", node_resq, 0);
    rb_ary_push(members, rb_str_new2("resq"));
    rb_define_method(rb_cRESBODY, "ensr", node_ensr, 0);
    rb_ary_push(members, rb_str_new2("ensr"));
  }
  {
    VALUE rb_cENSURE = rb_define_class_under(rb_cNode, "ENSURE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ENSURE] = rb_cENSURE;
    rb_iv_set(rb_cENSURE, "__member__", members);
    rb_define_singleton_method(rb_cENSURE, "members", node_s_members, 0);
    rb_define_method(rb_cENSURE, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cENSURE, "ensr", node_ensr, 0);
    rb_ary_push(members, rb_str_new2("ensr"));
  }
  {
    VALUE rb_cAND = rb_define_class_under(rb_cNode, "AND", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_AND] = rb_cAND;
    rb_iv_set(rb_cAND, "__member__", members);
    rb_define_singleton_method(rb_cAND, "members", node_s_members, 0);
    rb_define_method(rb_cAND, "first", node_1st, 0);
    rb_ary_push(members, rb_str_new2("first"));
    rb_define_method(rb_cAND, "second", node_2nd, 0);
    rb_ary_push(members, rb_str_new2("second"));
  }
  {
    VALUE rb_cOR = rb_define_class_under(rb_cNode, "OR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OR] = rb_cOR;
    rb_iv_set(rb_cOR, "__member__", members);
    rb_define_singleton_method(rb_cOR, "members", node_s_members, 0);
    rb_define_method(rb_cOR, "first", node_1st, 0);
    rb_ary_push(members, rb_str_new2("first"));
    rb_define_method(rb_cOR, "second", node_2nd, 0);
    rb_ary_push(members, rb_str_new2("second"));
  }
  {
    VALUE rb_cNOT = rb_define_class_under(rb_cNode, "NOT", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_NOT] = rb_cNOT;
    rb_iv_set(rb_cNOT, "__member__", members);
    rb_define_singleton_method(rb_cNOT, "members", node_s_members, 0);
    rb_define_method(rb_cNOT, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cDOT2 = rb_define_class_under(rb_cNode, "DOT2", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DOT2] = rb_cDOT2;
    rb_iv_set(rb_cDOT2, "__member__", members);
    rb_define_singleton_method(rb_cDOT2, "members", node_s_members, 0);
    rb_define_method(rb_cDOT2, "beg", node_beg, 0);
    rb_ary_push(members, rb_str_new2("beg"));
    rb_define_method(rb_cDOT2, "end", node_end, 0);
    rb_ary_push(members, rb_str_new2("end"));
    rb_define_method(rb_cDOT2, "state", node_state, 0);
    rb_ary_push(members, rb_str_new2("state"));
  }
  {
    VALUE rb_cDOT3 = rb_define_class_under(rb_cNode, "DOT3", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DOT3] = rb_cDOT3;
    rb_iv_set(rb_cDOT3, "__member__", members);
    rb_define_singleton_method(rb_cDOT3, "members", node_s_members, 0);
    rb_define_method(rb_cDOT3, "beg", node_beg, 0);
    rb_ary_push(members, rb_str_new2("beg"));
    rb_define_method(rb_cDOT3, "end", node_end, 0);
    rb_ary_push(members, rb_str_new2("end"));
    rb_define_method(rb_cDOT3, "state", node_state, 0);
    rb_ary_push(members, rb_str_new2("state"));
  }
  {
    VALUE rb_cFLIP2 = rb_define_class_under(rb_cNode, "FLIP2", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FLIP2] = rb_cFLIP2;
    rb_iv_set(rb_cFLIP2, "__member__", members);
    rb_define_singleton_method(rb_cFLIP2, "members", node_s_members, 0);
    rb_define_method(rb_cFLIP2, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
    rb_define_method(rb_cFLIP2, "beg", node_beg, 0);
    rb_ary_push(members, rb_str_new2("beg"));
    rb_define_method(rb_cFLIP2, "end", node_end, 0);
    rb_ary_push(members, rb_str_new2("end"));
  }
  {
    VALUE rb_cFLIP3 = rb_define_class_under(rb_cNode, "FLIP3", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FLIP3] = rb_cFLIP3;
    rb_iv_set(rb_cFLIP3, "__member__", members);
    rb_define_singleton_method(rb_cFLIP3, "members", node_s_members, 0);
    rb_define_method(rb_cFLIP3, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
    rb_define_method(rb_cFLIP3, "beg", node_beg, 0);
    rb_ary_push(members, rb_str_new2("beg"));
    rb_define_method(rb_cFLIP3, "end", node_end, 0);
    rb_ary_push(members, rb_str_new2("end"));
  }
  {
    VALUE rb_cRETURN = rb_define_class_under(rb_cNode, "RETURN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_RETURN] = rb_cRETURN;
    rb_iv_set(rb_cRETURN, "__member__", members);
    rb_define_singleton_method(rb_cRETURN, "members", node_s_members, 0);
    rb_define_method(rb_cRETURN, "stts", node_stts, 0);
    rb_ary_push(members, rb_str_new2("stts"));
  }
  {
    VALUE rb_cARGS = rb_define_class_under(rb_cNode, "ARGS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ARGS] = rb_cARGS;
    rb_iv_set(rb_cARGS, "__member__", members);
    rb_define_singleton_method(rb_cARGS, "members", node_s_members, 0);
    rb_define_method(rb_cARGS, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
    rb_define_method(rb_cARGS, "rest", node_rest, 0);
    rb_ary_push(members, rb_str_new2("rest"));
    rb_define_method(rb_cARGS, "opt", node_opt, 0);
    rb_ary_push(members, rb_str_new2("opt"));
  }
  {
    VALUE rb_cARGSCAT = rb_define_class_under(rb_cNode, "ARGSCAT", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ARGSCAT] = rb_cARGSCAT;
    rb_iv_set(rb_cARGSCAT, "__member__", members);
    rb_define_singleton_method(rb_cARGSCAT, "members", node_s_members, 0);
    rb_define_method(rb_cARGSCAT, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cARGSCAT, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cARGSPUSH = rb_define_class_under(rb_cNode, "ARGSPUSH", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ARGSPUSH] = rb_cARGSPUSH;
    rb_iv_set(rb_cARGSPUSH, "__member__", members);
    rb_define_singleton_method(rb_cARGSPUSH, "members", node_s_members, 0);
    rb_define_method(rb_cARGSPUSH, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cARGSPUSH, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cCALL = rb_define_class_under(rb_cNode, "CALL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CALL] = rb_cCALL;
    rb_iv_set(rb_cCALL, "__member__", members);
    rb_define_singleton_method(rb_cCALL, "members", node_s_members, 0);
    rb_define_method(rb_cCALL, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cCALL, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
    rb_define_method(rb_cCALL, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cFCALL = rb_define_class_under(rb_cNode, "FCALL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FCALL] = rb_cFCALL;
    rb_iv_set(rb_cFCALL, "__member__", members);
    rb_define_singleton_method(rb_cFCALL, "members", node_s_members, 0);
    rb_define_method(rb_cFCALL, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
    rb_define_method(rb_cFCALL, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cVCALL = rb_define_class_under(rb_cNode, "VCALL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_VCALL] = rb_cVCALL;
    rb_iv_set(rb_cVCALL, "__member__", members);
    rb_define_singleton_method(rb_cVCALL, "members", node_s_members, 0);
    rb_define_method(rb_cVCALL, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cSUPER = rb_define_class_under(rb_cNode, "SUPER", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_SUPER] = rb_cSUPER;
    rb_iv_set(rb_cSUPER, "__member__", members);
    rb_define_singleton_method(rb_cSUPER, "members", node_s_members, 0);
    rb_define_method(rb_cSUPER, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
  }
  {
    VALUE rb_cZSUPER = rb_define_class_under(rb_cNode, "ZSUPER", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ZSUPER] = rb_cZSUPER;
    rb_iv_set(rb_cZSUPER, "__member__", members);
    rb_define_singleton_method(rb_cZSUPER, "members", node_s_members, 0);
    rb_define_method(rb_cZSUPER, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
  }
  {
    VALUE rb_cSCOPE = rb_define_class_under(rb_cNode, "SCOPE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_SCOPE] = rb_cSCOPE;
    rb_iv_set(rb_cSCOPE, "__member__", members);
    rb_define_singleton_method(rb_cSCOPE, "members", node_s_members, 0);
    rb_define_method(rb_cSCOPE, "rval", node_rval, 0);
    rb_ary_push(members, rb_str_new2("rval"));
    rb_define_method(rb_cSCOPE, "tbl", node_tbl, 0);
    rb_ary_push(members, rb_str_new2("tbl"));
    rb_define_method(rb_cSCOPE, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cOP_ASGN1 = rb_define_class_under(rb_cNode, "OP_ASGN1", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OP_ASGN1] = rb_cOP_ASGN1;
    rb_iv_set(rb_cOP_ASGN1, "__member__", members);
    rb_define_singleton_method(rb_cOP_ASGN1, "members", node_s_members, 0);
    rb_define_method(rb_cOP_ASGN1, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cOP_ASGN1, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
    rb_define_method(rb_cOP_ASGN1, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cOP_ASGN2 = rb_define_class_under(rb_cNode, "OP_ASGN2", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OP_ASGN2] = rb_cOP_ASGN2;
    rb_iv_set(rb_cOP_ASGN2, "__member__", members);
    rb_define_singleton_method(rb_cOP_ASGN2, "members", node_s_members, 0);
    rb_define_method(rb_cOP_ASGN2, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cOP_ASGN2, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cOP_ASGN2, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cOP_ASGN_AND = rb_define_class_under(rb_cNode, "OP_ASGN_AND", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OP_ASGN_AND] = rb_cOP_ASGN_AND;
    rb_iv_set(rb_cOP_ASGN_AND, "__member__", members);
    rb_define_singleton_method(rb_cOP_ASGN_AND, "members", node_s_members, 0);
    rb_define_method(rb_cOP_ASGN_AND, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cOP_ASGN_AND, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cOP_ASGN_OR = rb_define_class_under(rb_cNode, "OP_ASGN_OR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_OP_ASGN_OR] = rb_cOP_ASGN_OR;
    rb_iv_set(rb_cOP_ASGN_OR, "__member__", members);
    rb_define_singleton_method(rb_cOP_ASGN_OR, "members", node_s_members, 0);
    rb_define_method(rb_cOP_ASGN_OR, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cOP_ASGN_OR, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cMASGN = rb_define_class_under(rb_cNode, "MASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MASGN] = rb_cMASGN;
    rb_iv_set(rb_cMASGN, "__member__", members);
    rb_define_singleton_method(rb_cMASGN, "members", node_s_members, 0);
    rb_define_method(rb_cMASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cMASGN, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cMASGN, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
  }
  {
    VALUE rb_cLASGN = rb_define_class_under(rb_cNode, "LASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_LASGN] = rb_cLASGN;
    rb_iv_set(rb_cLASGN, "__member__", members);
    rb_define_singleton_method(rb_cLASGN, "members", node_s_members, 0);
    rb_define_method(rb_cLASGN, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cLASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cLASGN, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
  {
    VALUE rb_cDASGN = rb_define_class_under(rb_cNode, "DASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DASGN] = rb_cDASGN;
    rb_iv_set(rb_cDASGN, "__member__", members);
    rb_define_singleton_method(rb_cDASGN, "members", node_s_members, 0);
    rb_define_method(rb_cDASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cDASGN, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cDASGN_CURR = rb_define_class_under(rb_cNode, "DASGN_CURR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DASGN_CURR] = rb_cDASGN_CURR;
    rb_iv_set(rb_cDASGN_CURR, "__member__", members);
    rb_define_singleton_method(rb_cDASGN_CURR, "members", node_s_members, 0);
    rb_define_method(rb_cDASGN_CURR, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cDASGN_CURR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cGASGN = rb_define_class_under(rb_cNode, "GASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_GASGN] = rb_cGASGN;
    rb_iv_set(rb_cGASGN, "__member__", members);
    rb_define_singleton_method(rb_cGASGN, "members", node_s_members, 0);
    rb_define_method(rb_cGASGN, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cGASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cGASGN, "entry", node_entry, 0);
    rb_ary_push(members, rb_str_new2("entry"));
  }
  {
    VALUE rb_cIASGN = rb_define_class_under(rb_cNode, "IASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_IASGN] = rb_cIASGN;
    rb_iv_set(rb_cIASGN, "__member__", members);
    rb_define_singleton_method(rb_cIASGN, "members", node_s_members, 0);
    rb_define_method(rb_cIASGN, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cIASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cCDECL = rb_define_class_under(rb_cNode, "CDECL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CDECL] = rb_cCDECL;
    rb_iv_set(rb_cCDECL, "__member__", members);
    rb_define_singleton_method(rb_cCDECL, "members", node_s_members, 0);
    rb_define_method(rb_cCDECL, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cCDECL, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
  }
  {
    VALUE rb_cCVDECL = rb_define_class_under(rb_cNode, "CVDECL", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CVDECL] = rb_cCVDECL;
    rb_iv_set(rb_cCVDECL, "__member__", members);
    rb_define_singleton_method(rb_cCVDECL, "members", node_s_members, 0);
    rb_define_method(rb_cCVDECL, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cCVDECL, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cCVASGN = rb_define_class_under(rb_cNode, "CVASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CVASGN] = rb_cCVASGN;
    rb_iv_set(rb_cCVASGN, "__member__", members);
    rb_define_singleton_method(rb_cCVASGN, "members", node_s_members, 0);
    rb_define_method(rb_cCVASGN, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cCVASGN, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cLVAR = rb_define_class_under(rb_cNode, "LVAR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_LVAR] = rb_cLVAR;
    rb_iv_set(rb_cLVAR, "__member__", members);
    rb_define_singleton_method(rb_cLVAR, "members", node_s_members, 0);
    rb_define_method(rb_cLVAR, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
    rb_define_method(rb_cLVAR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cDVAR = rb_define_class_under(rb_cNode, "DVAR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DVAR] = rb_cDVAR;
    rb_iv_set(rb_cDVAR, "__member__", members);
    rb_define_singleton_method(rb_cDVAR, "members", node_s_members, 0);
    rb_define_method(rb_cDVAR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cGVAR = rb_define_class_under(rb_cNode, "GVAR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_GVAR] = rb_cGVAR;
    rb_iv_set(rb_cGVAR, "__member__", members);
    rb_define_singleton_method(rb_cGVAR, "members", node_s_members, 0);
    rb_define_method(rb_cGVAR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
    rb_define_method(rb_cGVAR, "entry", node_entry, 0);
    rb_ary_push(members, rb_str_new2("entry"));
  }
  {
    VALUE rb_cIVAR = rb_define_class_under(rb_cNode, "IVAR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_IVAR] = rb_cIVAR;
    rb_iv_set(rb_cIVAR, "__member__", members);
    rb_define_singleton_method(rb_cIVAR, "members", node_s_members, 0);
    rb_define_method(rb_cIVAR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cCONST = rb_define_class_under(rb_cNode, "CONST", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CONST] = rb_cCONST;
    rb_iv_set(rb_cCONST, "__member__", members);
    rb_define_singleton_method(rb_cCONST, "members", node_s_members, 0);
    rb_define_method(rb_cCONST, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cCVAR = rb_define_class_under(rb_cNode, "CVAR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CVAR] = rb_cCVAR;
    rb_iv_set(rb_cCVAR, "__member__", members);
    rb_define_singleton_method(rb_cCVAR, "members", node_s_members, 0);
    rb_define_method(rb_cCVAR, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cBLOCK_ARG = rb_define_class_under(rb_cNode, "BLOCK_ARG", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BLOCK_ARG] = rb_cBLOCK_ARG;
    rb_iv_set(rb_cBLOCK_ARG, "__member__", members);
    rb_define_singleton_method(rb_cBLOCK_ARG, "members", node_s_members, 0);
    rb_define_method(rb_cBLOCK_ARG, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
  {
    VALUE rb_cCOLON2 = rb_define_class_under(rb_cNode, "COLON2", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_COLON2] = rb_cCOLON2;
    rb_iv_set(rb_cCOLON2, "__member__", members);
    rb_define_singleton_method(rb_cCOLON2, "members", node_s_members, 0);
    rb_define_method(rb_cCOLON2, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cCOLON2, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cCOLON3 = rb_define_class_under(rb_cNode, "COLON3", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_COLON3] = rb_cCOLON3;
    rb_iv_set(rb_cCOLON3, "__member__", members);
    rb_define_singleton_method(rb_cCOLON3, "members", node_s_members, 0);
    rb_define_method(rb_cCOLON3, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cNTH_REF = rb_define_class_under(rb_cNode, "NTH_REF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_NTH_REF] = rb_cNTH_REF;
    rb_iv_set(rb_cNTH_REF, "__member__", members);
    rb_define_singleton_method(rb_cNTH_REF, "members", node_s_members, 0);
    rb_define_method(rb_cNTH_REF, "nth", node_nth, 0);
    rb_ary_push(members, rb_str_new2("nth"));
    rb_define_method(rb_cNTH_REF, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
  {
    VALUE rb_cBACK_REF = rb_define_class_under(rb_cNode, "BACK_REF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BACK_REF] = rb_cBACK_REF;
    rb_iv_set(rb_cBACK_REF, "__member__", members);
    rb_define_singleton_method(rb_cBACK_REF, "members", node_s_members, 0);
    rb_define_method(rb_cBACK_REF, "nth", node_nth, 0);
    rb_ary_push(members, rb_str_new2("nth"));
    rb_define_method(rb_cBACK_REF, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
  {
    VALUE rb_cHASH = rb_define_class_under(rb_cNode, "HASH", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_HASH] = rb_cHASH;
    rb_iv_set(rb_cHASH, "__member__", members);
    rb_define_singleton_method(rb_cHASH, "members", node_s_members, 0);
    rb_define_method(rb_cHASH, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
  }
  {
    VALUE rb_cZARRAY = rb_define_class_under(rb_cNode, "ZARRAY", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ZARRAY] = rb_cZARRAY;
    rb_iv_set(rb_cZARRAY, "__member__", members);
    rb_define_singleton_method(rb_cZARRAY, "members", node_s_members, 0);
  }
  {
    VALUE rb_cARRAY = rb_define_class_under(rb_cNode, "ARRAY", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ARRAY] = rb_cARRAY;
    rb_iv_set(rb_cARRAY, "__member__", members);
    rb_define_singleton_method(rb_cARRAY, "members", node_s_members, 0);
    rb_define_method(rb_cARRAY, "alen", node_alen, 0);
    rb_ary_push(members, rb_str_new2("alen"));
    rb_define_method(rb_cARRAY, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
    rb_define_method(rb_cARRAY, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cSTR = rb_define_class_under(rb_cNode, "STR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_STR] = rb_cSTR;
    rb_iv_set(rb_cSTR, "__member__", members);
    rb_define_singleton_method(rb_cSTR, "members", node_s_members, 0);
    rb_define_method(rb_cSTR, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
  }
  {
    VALUE rb_cDREGX = rb_define_class_under(rb_cNode, "DREGX", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DREGX] = rb_cDREGX;
    rb_iv_set(rb_cDREGX, "__member__", members);
    rb_define_singleton_method(rb_cDREGX, "members", node_s_members, 0);
    rb_define_method(rb_cDREGX, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cDREGX, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
    rb_define_method(rb_cDREGX, "cflag", node_cflag, 0);
    rb_ary_push(members, rb_str_new2("cflag"));
  }
  {
    VALUE rb_cDREGX_ONCE = rb_define_class_under(rb_cNode, "DREGX_ONCE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DREGX_ONCE] = rb_cDREGX_ONCE;
    rb_iv_set(rb_cDREGX_ONCE, "__member__", members);
    rb_define_singleton_method(rb_cDREGX_ONCE, "members", node_s_members, 0);
    rb_define_method(rb_cDREGX_ONCE, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cDREGX_ONCE, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
    rb_define_method(rb_cDREGX_ONCE, "cflag", node_cflag, 0);
    rb_ary_push(members, rb_str_new2("cflag"));
  }
  {
    VALUE rb_cDSTR = rb_define_class_under(rb_cNode, "DSTR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DSTR] = rb_cDSTR;
    rb_iv_set(rb_cDSTR, "__member__", members);
    rb_define_singleton_method(rb_cDSTR, "members", node_s_members, 0);
    rb_define_method(rb_cDSTR, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cDSTR, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cDXSTR = rb_define_class_under(rb_cNode, "DXSTR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DXSTR] = rb_cDXSTR;
    rb_iv_set(rb_cDXSTR, "__member__", members);
    rb_define_singleton_method(rb_cDXSTR, "members", node_s_members, 0);
    rb_define_method(rb_cDXSTR, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cDXSTR, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cXSTR = rb_define_class_under(rb_cNode, "XSTR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_XSTR] = rb_cXSTR;
    rb_iv_set(rb_cXSTR, "__member__", members);
    rb_define_singleton_method(rb_cXSTR, "members", node_s_members, 0);
    rb_define_method(rb_cXSTR, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
  }
#if defined(C_ALLOCA) || RUBY_VERSION_CODE >= 190
  {
    VALUE rb_cALLOCA = rb_define_class_under(rb_cNode, "ALLOCA", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ALLOCA] = rb_cALLOCA;
    rb_iv_set(rb_cALLOCA, "__member__", members);
    rb_define_singleton_method(rb_cALLOCA, "members", node_s_members, 0);
    rb_define_method(rb_cALLOCA, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cALLOCA, "value", node_value, 0);
    rb_ary_push(members, rb_str_new2("value"));
    rb_define_method(rb_cALLOCA, "cnt", node_cnt, 0);
    rb_ary_push(members, rb_str_new2("cnt"));
  }
#endif
  {
    VALUE rb_cEVSTR = rb_define_class_under(rb_cNode, "EVSTR", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_EVSTR] = rb_cEVSTR;
    rb_iv_set(rb_cEVSTR, "__member__", members);
    rb_define_singleton_method(rb_cEVSTR, "members", node_s_members, 0);
    rb_define_method(rb_cEVSTR, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cLIT = rb_define_class_under(rb_cNode, "LIT", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_LIT] = rb_cLIT;
    rb_iv_set(rb_cLIT, "__member__", members);
    rb_define_singleton_method(rb_cLIT, "members", node_s_members, 0);
    rb_define_method(rb_cLIT, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
  }
  {
    VALUE rb_cATTRSET = rb_define_class_under(rb_cNode, "ATTRSET", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ATTRSET] = rb_cATTRSET;
    rb_iv_set(rb_cATTRSET, "__member__", members);
    rb_define_singleton_method(rb_cATTRSET, "members", node_s_members, 0);
    rb_define_method(rb_cATTRSET, "vid", node_vid, 0);
    rb_ary_push(members, rb_str_new2("vid"));
  }
  {
    VALUE rb_cDEFN = rb_define_class_under(rb_cNode, "DEFN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DEFN] = rb_cDEFN;
    rb_iv_set(rb_cDEFN, "__member__", members);
    rb_define_singleton_method(rb_cDEFN, "members", node_s_members, 0);
    rb_define_method(rb_cDEFN, "defn", node_defn, 0);
    rb_ary_push(members, rb_str_new2("defn"));
    rb_define_method(rb_cDEFN, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
    rb_define_method(rb_cDEFN, "noex", node_noex, 0);
    rb_ary_push(members, rb_str_new2("noex"));
  }
  {
    VALUE rb_cDEFS = rb_define_class_under(rb_cNode, "DEFS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DEFS] = rb_cDEFS;
    rb_iv_set(rb_cDEFS, "__member__", members);
    rb_define_singleton_method(rb_cDEFS, "members", node_s_members, 0);
    rb_define_method(rb_cDEFS, "defn", node_defn, 0);
    rb_ary_push(members, rb_str_new2("defn"));
    rb_define_method(rb_cDEFS, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cDEFS, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cUNDEF = rb_define_class_under(rb_cNode, "UNDEF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_UNDEF] = rb_cUNDEF;
    rb_iv_set(rb_cUNDEF, "__member__", members);
    rb_define_singleton_method(rb_cUNDEF, "members", node_s_members, 0);
    rb_define_method(rb_cUNDEF, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
  }
  {
    VALUE rb_cALIAS = rb_define_class_under(rb_cNode, "ALIAS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ALIAS] = rb_cALIAS;
    rb_iv_set(rb_cALIAS, "__member__", members);
    rb_define_singleton_method(rb_cALIAS, "members", node_s_members, 0);
    rb_define_method(rb_cALIAS, "first", node_1st, 0);
    rb_ary_push(members, rb_str_new2("first"));
    rb_define_method(rb_cALIAS, "second", node_2nd, 0);
    rb_ary_push(members, rb_str_new2("second"));
  }
  {
    VALUE rb_cVALIAS = rb_define_class_under(rb_cNode, "VALIAS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_VALIAS] = rb_cVALIAS;
    rb_iv_set(rb_cVALIAS, "__member__", members);
    rb_define_singleton_method(rb_cVALIAS, "members", node_s_members, 0);
    rb_define_method(rb_cVALIAS, "first", node_1st, 0);
    rb_ary_push(members, rb_str_new2("first"));
    rb_define_method(rb_cVALIAS, "second", node_2nd, 0);
    rb_ary_push(members, rb_str_new2("second"));
  }
  {
    VALUE rb_cSCLASS = rb_define_class_under(rb_cNode, "SCLASS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_SCLASS] = rb_cSCLASS;
    rb_iv_set(rb_cSCLASS, "__member__", members);
    rb_define_singleton_method(rb_cSCLASS, "members", node_s_members, 0);
    rb_define_method(rb_cSCLASS, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cSCLASS, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cDEFINED = rb_define_class_under(rb_cNode, "DEFINED", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DEFINED] = rb_cDEFINED;
    rb_iv_set(rb_cDEFINED, "__member__", members);
    rb_define_singleton_method(rb_cDEFINED, "members", node_s_members, 0);
    rb_define_method(rb_cDEFINED, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
  }
  {
    VALUE rb_cIFUNC = rb_define_class_under(rb_cNode, "IFUNC", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_IFUNC] = rb_cIFUNC;
    rb_iv_set(rb_cIFUNC, "__member__", members);
    rb_define_singleton_method(rb_cIFUNC, "members", node_s_members, 0);
    rb_define_method(rb_cIFUNC, "cfnc", node_cfnc, 0);
    rb_ary_push(members, rb_str_new2("cfnc"));
    rb_define_method(rb_cIFUNC, "tval", node_tval, 0);
    rb_ary_push(members, rb_str_new2("tval"));
    rb_define_method(rb_cIFUNC, "argc", node_argc, 0);
    rb_ary_push(members, rb_str_new2("argc"));
  }
  {
    VALUE rb_cCFUNC = rb_define_class_under(rb_cNode, "CFUNC", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CFUNC] = rb_cCFUNC;
    rb_iv_set(rb_cCFUNC, "__member__", members);
    rb_define_singleton_method(rb_cCFUNC, "members", node_s_members, 0);
    rb_define_method(rb_cCFUNC, "cfnc", node_cfnc, 0);
    rb_ary_push(members, rb_str_new2("cfnc"));
    rb_define_method(rb_cCFUNC, "tval", node_tval, 0);
    rb_ary_push(members, rb_str_new2("tval"));
    rb_define_method(rb_cCFUNC, "argc", node_argc, 0);
    rb_ary_push(members, rb_str_new2("argc"));
  }
  {
    VALUE rb_cFBODY = rb_define_class_under(rb_cNode, "FBODY", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_FBODY] = rb_cFBODY;
    rb_iv_set(rb_cFBODY, "__member__", members);
    rb_define_singleton_method(rb_cFBODY, "members", node_s_members, 0);
    rb_define_method(rb_cFBODY, "orig", node_orig, 0);
    rb_ary_push(members, rb_str_new2("orig"));
    rb_define_method(rb_cFBODY, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
    rb_define_method(rb_cFBODY, "head", node_head, 0);
    rb_ary_push(members, rb_str_new2("head"));
  }
  {
    VALUE rb_cCREF = rb_define_class_under(rb_cNode, "CREF", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CREF] = rb_cCREF;
    rb_iv_set(rb_cCREF, "__member__", members);
    rb_define_singleton_method(rb_cCREF, "members", node_s_members, 0);
    rb_define_method(rb_cCREF, "clss", node_clss, 0);
    rb_ary_push(members, rb_str_new2("clss"));
    rb_define_method(rb_cCREF, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
    rb_define_method(rb_cCREF, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cBMETHOD = rb_define_class_under(rb_cNode, "BMETHOD", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_BMETHOD] = rb_cBMETHOD;
    rb_iv_set(rb_cBMETHOD, "__member__", members);
    rb_define_singleton_method(rb_cBMETHOD, "members", node_s_members, 0);
    rb_define_method(rb_cBMETHOD, "cval", node_cval, 0);
    rb_ary_push(members, rb_str_new2("cval"));
  }
  {
    VALUE rb_cMEMO = rb_define_class_under(rb_cNode, "MEMO", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MEMO] = rb_cMEMO;
    rb_iv_set(rb_cMEMO, "__member__", members);
    rb_define_singleton_method(rb_cMEMO, "members", node_s_members, 0);
    rb_define_method(rb_cMEMO, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cMEMO, "tval", node_tval, 0);
    rb_ary_push(members, rb_str_new2("tval"));
  }
  {
    VALUE rb_cDMETHOD = rb_define_class_under(rb_cNode, "DMETHOD", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DMETHOD] = rb_cDMETHOD;
    rb_iv_set(rb_cDMETHOD, "__member__", members);
    rb_define_singleton_method(rb_cDMETHOD, "members", node_s_members, 0);
    rb_define_method(rb_cDMETHOD, "cval", node_cval, 0);
    rb_ary_push(members, rb_str_new2("cval"));
  }
  {
    VALUE rb_cNEWLINE = rb_define_class_under(rb_cNode, "NEWLINE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_NEWLINE] = rb_cNEWLINE;
    rb_iv_set(rb_cNEWLINE, "__member__", members);
    rb_define_singleton_method(rb_cNEWLINE, "members", node_s_members, 0);
    rb_define_method(rb_cNEWLINE, "nth", node_nth, 0);
    rb_ary_push(members, rb_str_new2("nth"));
    rb_define_method(rb_cNEWLINE, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cCLASS = rb_define_class_under(rb_cNode, "CLASS", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_CLASS] = rb_cCLASS;
    rb_iv_set(rb_cCLASS, "__member__", members);
    rb_define_singleton_method(rb_cCLASS, "members", node_s_members, 0);
    rb_define_method(rb_cCLASS, "super", node_super, 0);
    rb_ary_push(members, rb_str_new2("super"));
    rb_define_method(rb_cCLASS, "cpath", node_cpath, 0);
    rb_ary_push(members, rb_str_new2("cpath"));
    rb_define_method(rb_cCLASS, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cMODULE = rb_define_class_under(rb_cNode, "MODULE", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_MODULE] = rb_cMODULE;
    rb_iv_set(rb_cMODULE, "__member__", members);
    rb_define_singleton_method(rb_cMODULE, "members", node_s_members, 0);
    rb_define_method(rb_cMODULE, "cpath", node_cpath, 0);
    rb_ary_push(members, rb_str_new2("cpath"));
    rb_define_method(rb_cMODULE, "body", node_body, 0);
    rb_ary_push(members, rb_str_new2("body"));
  }
  {
    VALUE rb_cDSYM = rb_define_class_under(rb_cNode, "DSYM", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_DSYM] = rb_cDSYM;
    rb_iv_set(rb_cDSYM, "__member__", members);
    rb_define_singleton_method(rb_cDSYM, "members", node_s_members, 0);
    rb_define_method(rb_cDSYM, "lit", node_lit, 0);
    rb_ary_push(members, rb_str_new2("lit"));
    rb_define_method(rb_cDSYM, "next", node_next, 0);
    rb_ary_push(members, rb_str_new2("next"));
  }
  {
    VALUE rb_cATTRASGN = rb_define_class_under(rb_cNode, "ATTRASGN", rb_cNode);
    members = rb_ary_new();
    rb_cNodeSubclass[NODE_ATTRASGN] = rb_cATTRASGN;
    rb_iv_set(rb_cATTRASGN, "__member__", members);
    rb_define_singleton_method(rb_cATTRASGN, "members", node_s_members, 0);
    rb_define_method(rb_cATTRASGN, "mid", node_mid, 0);
    rb_ary_push(members, rb_str_new2("mid"));
    rb_define_method(rb_cATTRASGN, "recv", node_recv, 0);
    rb_ary_push(members, rb_str_new2("recv"));
    rb_define_method(rb_cATTRASGN, "args", node_args, 0);
    rb_ary_push(members, rb_str_new2("args"));
  }
}

