Node_Samples = {
  :method      => "",
  :fbody       => "",
  # cfunc not dumpable
  :ifunc       => "",
  :scope       => "",
  :block       => "",
  :if          => "if true then 1 end",
  :case        => "case 1 when 1 then true end",
  :when        => "case 1 when 1 then true end",
  :opt_n       => "",
  :while       => "a = 0; while a != 1; a += 1; end",
  :until       => "a = 0; until a == 1; a += 1; end",
  :iter        => "",
  :for         => "a = 0; for i in 1..42; a += 1; end; a",
  :break       => "a = 24; for i in 1..42; break if i == 24; a += 1; end; a",
  :next        => "[1].each { next }",
  :redo        => "a = false; [1].each { if not a; a = true; redo; end }",
  :retry       => "",
  :begin       => "begin; rescue; end",
  :rescue      => "begin; raise 'foo'; rescue; end",
  :resbody     => "begin; raise 'foo'; rescue; end",
  :ensure      => "begin; ensure; 10; end",
  :and         => "true and false",
  :or          => "if 1 or 2 then 3; end",
  :not         => "not false",
  :masgn       => "a, b, c = 4, 5, 6",
  :lasgn       => "",
  :dasgn       => "",
  :dasgn_curr  => "",
  :gasgn       => "$a = 1",
  :iasgn       => "@a = 1",
  :cdecl       => "",
  :cvasgn      => "",
  :cvdecl      => "",
  :op_asgn1    => "{}[10] = 1",
  :op_asgn2    => "{}.default = 1",
  :op_asgn_and => "",
  :op_asgn_or  => "",
  :call        => "",
  :fcall       => "",
  :vcall       => "",
  :super       => "super()",
  :zsuper      => "super",
  :array       => "",
  :zarray      => "",
  :hash        => "{ 1 => 2 }",
  :return      => "",
  :yield       => "yield",
  :lvar        => "",
  :dvar        => "",
  :gvar        => "$a == 1",
  :ivar        => "@a == 42", # must be diff from iasgn
  :const       => "",
  :cvar        => "",
  :cvar2       => "",
  :nth_ref     => "",
  :back_ref    => "",
  :match       => "",
  :match2      => "",
  :match3      => "",
  :lit         => "17",
  :str         => "\"foo\"",
  :dstr        => "\"#{8}\"",
  :xstr        => "",
  :dxstr       => "",
  :evstr       => "",
  :dregx       => "",
  :dregx_once  => "",
  :args        => "",
  :argscat     => "",
  :argspush    => "",
  :restargs    => "",
  :block_arg   => "",
  :block_pass  => "",
  :defn        => "",
  :defs        => "",
  :alias       => "",
  :valias      => "",
  :undef       => "",
  :class       => "",
  :module      => "",
  :sclass      => "class << self; end",
  :colon2      => "Kernel::Object",
  :colon3      => "::Object",
  :cref        => "",
  :dot2        => "1..2",
  :dot3        => "2...3",
  :flip2       => "if 'foo' =~ /foo/ .. /bar/; end", # TODO
  :flip3       => "if 'foo' =~ /foo/ ... /bar/; end", # TODO
  :attrset     => "",
  :self        => "self.nil?",
  :nil         => "nil",
  :true        => "true",
  :false       => "false",
  :defined     => "a = 1; defined?(a)",
  :newline     => "\n",
  :postexe     => "",
  :alloca      => "",
  :dmethod     => "",
  :bmethod     => "",
  :memo        => "",
  :node_last   => "",

  # These cause a crash:
  # self.class.ancestors.include?(Object)
  # Object === self
}

