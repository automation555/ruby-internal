# A code obfuscator for Ruby.  It works by getting a dump of the node
# tree, then wrapping it with a lightweight loader.
#
# NOTE: This code is for demonstration purposes only!  It has not been
# extensively tested, and I cannot guarantee that it works in all cases
# (I don't even know what the corner cases are).  If you wish to use
# this in a commercial product, please verify that the code is correctly
# loaded.
#
# Usage:
#
#   $ cat test.rb
#   def foo
#     1+1
#   end
#   
#   puts foo()
#
#   $ ruby -rnwobfusc test.rb > test2.rb
#   $ ruby test2.rb
#   2

require 'nodewrap'

def obfusc(begin_nodes, n)
  # The outputted code must do the following:
  #   1. Evaluate each of the begin nodes (BEGIN {...}).
  #   2. Evaluate the main node.
  puts <<-END
require 'nodewrap'
begin_nodes = Marshal.load(#{Marshal.dump(begin_nodes).inspect})
n = Marshal.load(#{Marshal.dump(n).inspect})
begin_nodes.each do |node|
  node.eval(self)
end
n.eval(self)
  END
end

begin_nodes = []
set_trace_func proc {
  n = $ruby_eval_tree
  if $ruby_eval_tree_begin then
    n.pretty_print($stderr)
    begin_nodes << $ruby_eval_tree_begin
  end
  if n then
    # TODO: Do I need to include ruby_dyna_vars?
    obfusc(begin_nodes, n)
    exit
  end
}
