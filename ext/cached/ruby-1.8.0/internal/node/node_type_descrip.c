#include "node_type_descrip.h"
#include "version.h"

Node_Type_Descrip node_type_descrips_unsorted[] = {
  { NODE_DASGN_CURR, NEN_VALUE, NEN_VID, NEN_NONE, "DASGN_CURR" },
  { NODE_UNTIL, NEN_BODY, NEN_COND, NEN_STATE, "UNTIL" },
  { NODE_BACK_REF, NEN_NTH, NEN_CNT, NEN_NONE, "BACK_REF" },
  { NODE_ARGS, NEN_REST, NEN_OPT, NEN_CNT, "ARGS" },
  { NODE_OP_ASGN_AND, NEN_VALUE, NEN_RECV, NEN_NONE, "OP_ASGN_AND" },
  { NODE_RETURN, NEN_STTS, NEN_NONE, NEN_NONE, "RETURN" },
  { NODE_RESCUE, NEN_HEAD, NEN_ELSE, NEN_RESQ, "RESCUE" },
  { NODE_LIT, NEN_LIT, NEN_NONE, NEN_NONE, "LIT" },
  { NODE_SELF, NEN_NONE, NEN_NONE, NEN_NONE, "SELF" },
  { NODE_IFUNC, NEN_CFNC, NEN_TVAL, NEN_STATE, "IFUNC" },
  { NODE_ZSUPER, NEN_NONE, NEN_NONE, NEN_NONE, "ZSUPER" },
  { NODE_WHILE, NEN_BODY, NEN_COND, NEN_STATE, "WHILE" },
  { NODE_BLOCK, NEN_HEAD, NEN_NEXT, NEN_NONE, "BLOCK" },
  { NODE_FCALL, NEN_ARGS, NEN_MID, NEN_NONE, "FCALL" },
  { NODE_DREGX_ONCE, NEN_LIT, NEN_CFLAG, NEN_NEXT, "DREGX_ONCE" },
  { NODE_ARGSPUSH, NEN_HEAD, NEN_BODY, NEN_NONE, "ARGSPUSH" },
  { NODE_OPT_N, NEN_BODY, NEN_NONE, NEN_NONE, "OPT_N" },
  { NODE_OP_ASGN2_ARG, NEN_VID, NEN_AID, NEN_MID, "OP_ASGN2_ARG" },
  { NODE_COLON2, NEN_HEAD, NEN_MID, NEN_NONE, "COLON2" },
  { NODE_FBODY, NEN_HEAD, NEN_ORIG, NEN_MID, "FBODY" },
  { NODE_COLON3, NEN_MID, NEN_NONE, NEN_NONE, "COLON3" },
  { NODE_ITER, NEN_BODY, NEN_ITER, NEN_VAR, "ITER" },
  { NODE_NOT, NEN_BODY, NEN_NONE, NEN_NONE, "NOT" },
  { NODE_SUPER, NEN_ARGS, NEN_NONE, NEN_NONE, "SUPER" },
  { NODE_FALSE, NEN_NONE, NEN_NONE, NEN_NONE, "FALSE" },
  { NODE_CALL, NEN_ARGS, NEN_MID, NEN_RECV, "CALL" },
  { NODE_OP_ASGN_OR, NEN_AID, NEN_VALUE, NEN_RECV, "OP_ASGN_OR" },
  { NODE_TRUE, NEN_NONE, NEN_NONE, NEN_NONE, "TRUE" },
  { NODE_MASGN, NEN_ARGS, NEN_HEAD, NEN_VALUE, "MASGN" },
  { NODE_EVSTR, NEN_BODY, NEN_NONE, NEN_NONE, "EVSTR" },
  { NODE_MATCH, NEN_LIT, NEN_VALUE, NEN_NONE, "MATCH" },
  { NODE_ENSURE, NEN_HEAD, NEN_ENSR, NEN_NONE, "ENSURE" },
  { NODE_CDECL, NEN_VALUE, NEN_VID, NEN_NONE, "CDECL" },
  { NODE_DXSTR, NEN_LIT, NEN_NEXT, NEN_NONE, "DXSTR" },
  { NODE_VCALL, NEN_MID, NEN_NONE, NEN_NONE, "VCALL" },
  { NODE_ZARRAY, NEN_NONE, NEN_NONE, NEN_NONE, "ZARRAY" },
  { NODE_BLOCK_ARG, NEN_CNT, NEN_NONE, NEN_NONE, "BLOCK_ARG" },
  { NODE_AND, NEN_1ST, NEN_2ND, NEN_NONE, "AND" },
  { NODE_SCLASS, NEN_BODY, NEN_RECV, NEN_NONE, "SCLASS" },
  { NODE_DEFINED, NEN_HEAD, NEN_NONE, NEN_NONE, "DEFINED" },
  { NODE_VALIAS, NEN_1ST, NEN_2ND, NEN_NONE, "VALIAS" },
  { NODE_REDO, NEN_NONE, NEN_NONE, NEN_NONE, "REDO" },
  { NODE_BLOCK_PASS, NEN_BODY, NEN_ITER, NEN_NONE, "BLOCK_PASS" },
  { NODE_DREGX, NEN_LIT, NEN_CFLAG, NEN_NEXT, "DREGX" },
  { NODE_DEFN, NEN_MID, NEN_DEFN, NEN_NOEX, "DEFN" },
  { NODE_SCOPE, NEN_TBL, NEN_RVAL, NEN_NEXT, "SCOPE" },
  { NODE_DSYM, NEN_LIT, NEN_NEXT, NEN_NONE, "DSYM" },
  { NODE_BMETHOD, NEN_CVAL, NEN_NONE, NEN_NONE, "BMETHOD" },
  { NODE_IF, NEN_COND, NEN_BODY, NEN_ELSE, "IF" },
  { NODE_WHEN, NEN_HEAD, NEN_BODY, NEN_NEXT, "WHEN" },
  { NODE_ARGSCAT, NEN_HEAD, NEN_BODY, NEN_NONE, "ARGSCAT" },
  { NODE_HASH, NEN_HEAD, NEN_NONE, NEN_NONE, "HASH" },
#ifdef HAVE_NODE_ALLOCA
  { NODE_ALLOCA, NEN_CFNC, NEN_VALUE, NEN_CNT, "ALLOCA" },
#endif
  { NODE_OP_ASGN1, NEN_ARGS, NEN_MID, NEN_RECV, "OP_ASGN1" },
  { NODE_CASE, NEN_HEAD, NEN_BODY, NEN_NEXT, "CASE" },
  { NODE_CFUNC, NEN_CFNC, NEN_ARGC, NEN_NONE, "CFUNC" },
  { NODE_OP_ASGN2, NEN_VALUE, NEN_NEXT, NEN_RECV, "OP_ASGN2" },
  { NODE_DEFS, NEN_MID, NEN_DEFN, NEN_RECV, "DEFS" },
  { NODE_LASGN, NEN_VALUE, NEN_VID, NEN_CNT, "LASGN" },
  { NODE_POSTEXE, NEN_NONE, NEN_NONE, NEN_NONE, "POSTEXE" },
  { NODE_STR, NEN_LIT, NEN_NONE, NEN_NONE, "STR" },
  { NODE_ATTRSET, NEN_VID, NEN_NONE, NEN_NONE, "ATTRSET" },
  { NODE_MEMO, NEN_NONE, NEN_NONE, NEN_NONE, "MEMO" },
  { NODE_XSTR, NEN_LIT, NEN_NONE, NEN_NONE, "XSTR" },
  { NODE_RESBODY, NEN_HEAD, NEN_ARGS, NEN_BODY, "RESBODY" },
  { NODE_ALIAS, NEN_1ST, NEN_2ND, NEN_NONE, "ALIAS" },
  { NODE_ARRAY, NEN_HEAD, NEN_ALEN, NEN_NEXT, "ARRAY" },
  { NODE_NEXT, NEN_STTS, NEN_NONE, NEN_NONE, "NEXT" },
  { NODE_GASGN, NEN_VALUE, NEN_VID, NEN_ENTRY, "GASGN" },
  { NODE_CVDECL, NEN_VALUE, NEN_VID, NEN_NONE, "CVDECL" },
  { NODE_ATTRASGN, NEN_ARGS, NEN_MID, NEN_RECV, "ATTRASGN" },
  { NODE_UNDEF, NEN_MID, NEN_NONE, NEN_NONE, "UNDEF" },
  { NODE_BREAK, NEN_STTS, NEN_NONE, NEN_NONE, "BREAK" },
  { NODE_NEWLINE, NEN_NEXT, NEN_NONE, NEN_NONE, "NEWLINE" },
  { NODE_NTH_REF, NEN_NTH, NEN_CNT, NEN_NONE, "NTH_REF" },
  { NODE_DOT2, NEN_BEG, NEN_END, NEN_STATE, "DOT2" },
  { NODE_GVAR, NEN_VID, NEN_ENTRY, NEN_NONE, "GVAR" },
  { NODE_DOT3, NEN_BEG, NEN_END, NEN_STATE, "DOT3" },
  { NODE_CVASGN, NEN_VALUE, NEN_VID, NEN_NONE, "CVASGN" },
  { NODE_CREF, NEN_BODY, NEN_NEXT, NEN_CLSS, "CREF" },
  { NODE_SPLAT, NEN_HEAD, NEN_NONE, NEN_NONE, "SPLAT" },
  { NODE_MODULE, NEN_CPATH, NEN_BODY, NEN_NONE, "MODULE" },
  { NODE_FOR, NEN_BODY, NEN_ITER, NEN_VAR, "FOR" },
  { NODE_DVAR, NEN_VID, NEN_NONE, NEN_NONE, "DVAR" },
  { NODE_DSTR, NEN_LIT, NEN_NEXT, NEN_NONE, "DSTR" },
  { NODE_SVALUE, NEN_HEAD, NEN_NONE, NEN_NONE, "SVALUE" },
  { NODE_RETRY, NEN_NONE, NEN_NONE, NEN_NONE, "RETRY" },
  { NODE_CLASS, NEN_CPATH, NEN_BODY, NEN_SUPER, "CLASS" },
  { NODE_DASGN, NEN_VALUE, NEN_VID, NEN_NONE, "DASGN" },
  { NODE_NIL, NEN_NONE, NEN_NONE, NEN_NONE, "NIL" },
  { NODE_RESTARY2, NEN_HEAD, NEN_NONE, NEN_NONE, "RESTARY2" },
  { NODE_IVAR, NEN_VID, NEN_NONE, NEN_NONE, "IVAR" },
  { NODE_LVAR, NEN_VID, NEN_CNT, NEN_NONE, "LVAR" },
  { NODE_MATCH2, NEN_VALUE, NEN_RECV, NEN_NONE, "MATCH2" },
  { NODE_FLIP2, NEN_BEG, NEN_END, NEN_CNT, "FLIP2" },
  { NODE_CONST, NEN_VID, NEN_NONE, NEN_NONE, "CONST" },
  { NODE_RESTARY, NEN_HEAD, NEN_NONE, NEN_NONE, "RESTARY" },
  { NODE_METHOD, NEN_BODY, NEN_NOEX, NEN_NONE, "METHOD" },
  { NODE_MATCH3, NEN_VALUE, NEN_RECV, NEN_NONE, "MATCH3" },
  { NODE_IASGN, NEN_VALUE, NEN_VID, NEN_NONE, "IASGN" },
  { NODE_DMETHOD, NEN_CVAL, NEN_NONE, NEN_NONE, "DMETHOD" },
  { NODE_FLIP3, NEN_BEG, NEN_END, NEN_CNT, "FLIP3" },
  { NODE_OR, NEN_1ST, NEN_2ND, NEN_NONE, "OR" },
  { NODE_CVAR, NEN_VID, NEN_NONE, NEN_NONE, "CVAR" },
  { NODE_YIELD, NEN_HEAD, NEN_STATE, NEN_NONE, "YIELD" },
  { NODE_BEGIN, NEN_BODY, NEN_NONE, NEN_NONE, "BEGIN" },
  { NODE_LAST, NEN_NONE, NEN_NONE, NEN_NONE, "LAST" },
};

static Node_Type_Descrip * node_type_descrips[NUM_NODE_TYPE_DESCRIPS];
static int node_type_descrips_initialized = 0;

static void init_node_type_descrips()
{
  if(!node_type_descrips_initialized)
  {
    Node_Type_Descrip * descrip;
    memset(node_type_descrips, 0, sizeof(node_type_descrips));
    for(descrip = node_type_descrips_unsorted;
        descrip->nt != NODE_LAST;
        ++descrip)
    {
      if(node_type_descrips[descrip->nt])
      {
        rb_raise(rb_eRuntimeError, "duplicate entry for node type %d (%s is also %s)\n", descrip->nt, descrip->name, node_type_descrips[descrip->nt]->name);
      }
      else
      {
        node_type_descrips[descrip->nt] = descrip;
      }
    }
    node_type_descrips_initialized = 1;
  }
}

/* Given a node, find out the types of the three elements it contains */
Node_Type_Descrip const * node_type_descrip(enum node_type nt)
{
  init_node_type_descrips();
  if(node_type_descrips[nt])
  {
    return node_type_descrips[nt];
  }
  else
  {
    rb_raise(rb_eArgError, "Unknown node type %d", nt);
  }
}

