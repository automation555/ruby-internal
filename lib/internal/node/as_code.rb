require 'internal/node'
require 'internal/node/as_expression'
require 'internal/object/as_code'
require 'rbconfig'

class Node
  public

  def as_code(indent=0, *args)
    return as_code_impl(self, indent, *args)
  end

  private

  def as_code_impl(node, indent, *args)
    # default -- most code is just an expression
    expression = node.as_expression(*args)
    if not expression.nil? then
      return "#{'  '*indent}#{expression}"
    else
      return nil
    end
  end

  class << self
    def define_code(klass, &block)
      if const_defined?(klass) then
        const_get(klass).instance_eval { define_method(:as_code_impl, &block) }
      end
    end
  end

  define_code(:IF) do |node, indent|
    if node.body.class == Node::BLOCK or
       node.else.class == Node::BLOCK then
      s = "#{'  '*indent}if #{node.cond.as_expression} then\n"
      s << "#{'  '*indent}#{node.body.as_code(indent+1)}\n"
      if node.else then
        s << "#{'  '*indent}else\n"
        s << "#{'  '*indent}#{node.else.as_code(indent+1)}\n"
      end
      s << "#{'  '*indent}end"
      s
    else
      "#{'  '*indent}#{node.as_expression}"
    end
  end

  define_code(:BLOCK) do |node, indent|
    a = node.to_a
    if a[0].class == Node::DASGN_CURR then
      vardefs = a[0]
      while vardefs.class == Node::DASGN_CURR do
        vardefs = vardefs.value
      end
      if not vardefs then
        # ignore variable definitions
        a.shift
      end
    end
    lines = a.map { |n| n.as_code(indent) }
    lines.reject! { |e| e.nil? }
    if lines.size == 0 then
      "#{'  '*indent}nil"
    else
      lines.join("\n")
    end
  end

  define_code(:ITER) do |node, indent|
    "#{'  '*indent}#{node.iter.as_expression} {\n" +
    "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
    "#{'  '*indent}}"
  end

  define_code(:WHILE) do |node, indent|
    if node.state == 1 then
      "#{'  '*indent}while #{node.cond.as_expression} do\n" +
      "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
      "#{'  '*indent}end"
    else
      "#{'  '*indent}begin\n" +
      "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
      "#{'  '*indent}end while #{node.cond.as_expression}"
    end
  end

  define_code(:UNTIL) do |node, indent|
    if node.state == 1 then
      "#{'  '*indent}until #{node.cond.as_expression} do\n" +
      "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
      "#{'  '*indent}end"
    else
      "#{'  '*indent}begin\n" +
      "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
      "#{'  '*indent}end until #{node.cond.as_expression}"
    end
  end

  define_code(:BEGIN) do |node, indent|
    if node.body.class == Node::RESCUE or
       node.body.class == Node::ENSURE then
      s = "#{'  '*indent}begin\n" +
      "#{node.body.as_code(indent+1, true)}\n" +
      "#{'  '*indent}end"
    elsif node.body then
      "#{'  '*indent}begin\n" +
      "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
      "#{'  '*indent}end"
    else
      "#{'  '*indent}begin\n" +
      "#{'  '*indent}end\n"
    end
  end

  def self.begin_end(indent, have_begin)
    s = ''
    if not have_begin then
      s << "#{'  '*indent}begin\n"
      indent += 1
    end
    yield s, indent, true
    if not have_begin then
      indent -= 1
      s << "\n#{'  '*indent}end"
    end
    return s
  end

  define_code(:ENSURE) do |node, indent, *args|
    begin_ensure = args[0] || false
    Node.begin_end(indent, begin_ensure) do |s, indent_, begin_ensure|
      if node.head then
        s << "#{node.head.as_code(indent_)}\n"
      end
      s << "#{'  '*(indent_-1)}ensure\n"
      s << "#{node.ensr.as_code(indent_)}"
    end
  end

  define_code(:RESCUE) do |node, indent, *args|
    begin_rescue = args[0] || false
    Node.begin_end(indent, begin_rescue) do |s, indent_, begin_rescue|
      if node.head then
        if begin_rescue then
          s << "#{node.head.as_code(indent_)}\n"
          s << "#{'  '*(indent_-1)}rescue #{node.resq.as_code(indent_+1, begin_rescue)}"
        else
          s << "#{node.head.as_expression} rescue #{node.resq.as_expression(begin_rescue)}"
        end
      else
        s << "rescue #{node.resq.as_expression(begin_rescue)}"
      end
    end
  end

  define_code(:RESBODY) do |node, indent, *args|
    begin_rescue = args[0] || false
    if begin_rescue then
      if node.args then
        a = node.args.to_a.map { |n| n.as_expression }
        "#{a.join(', ')}\n" +
        "#{'  '*indent}#{node.body.as_code(indent+1)}"
      else
        node.body ? "\n#{'  '*indent}#{node.body.as_code(indent+1)}" : ''
      end
    else
      # TODO: assuming node.args is false...
      node.body ? node.body.as_code : ''
    end
  end

  define_code(:NEWLINE) do |node, indent|
    node.next.as_code(indent)
  end

  define_code(:CASE) do |node, indent|
    "#{'  '*indent}case #{node.head.as_expression}\n" +
    "#{node.body.as_code(indent)}end"
  end

  define_code(:WHEN) do |node, indent|
    args = node.head.to_a.map { |n| n.as_expression }
    s = "#{'  '*indent}when #{args.join(', ')}\n"
    if node.body then
      s << "#{'  '*indent}#{node.body.as_code(indent+1)}; "
    end
    if node.next then
      s << node.next.as_code
    end
    s
  end

  define_code(:CLASS) do |node, indent|
    s_super = node.super ? " < #{node.super.as_expression}" : ''
    if node.respond_to?(:cpath) then
      path = node.cpath.as_expression
    else
      path = node.cname
    end
    "#{'  '*indent}class #{path}#{s_super}\n" +
    "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
    "#{'  '*indent}end"
  end

  define_code(:SCLASS) do |node, indent|
    "#{'  '*indent}class << #{node.recv.as_expression}\n" +
    "#{'  '*indent}#{node.body.as_code(indent+1)}\n" +
    "#{'  '*indent}end"
  end

  define_code(:DEFN) do |node, indent|
    # TODO: what to do about noex?
    "#{'  '*indent}def #{node.mid}\n" +
    "#{'  '*indent}#{node.next.as_code(indent+1)}\n" +
    "#{'  '*indent}end"
  end

  define_code(:DEFS) do |node, indent|
    "#{'  '*indent}def #{node.recv.as_expression}.#{node.mid}\n" +
    "#{'  '*indent}#{node.next.as_code(indent+1)}\n" +
    "#{'  '*indent}end"
  end

  define_code(:SCOPE) do |node, indent|
    case node.next
    when nil then ''
    when Node::ARGS then "#{'  '*indent}nil"
    when Node::BLOCK_ARG then "#{'  '*indent}nil"
    else node.next.as_code(indent)
    end
  end
end
