# Require this file to get a node pretty-printer.
#

require 'nodewrap'

module Noex
  # Return a string representing the given integer as a noex value.
  #
  def stringify(noex)
    Noex.constants.each do |constant|
      return constant if const_get(constant) == noex
    end
    return noex.to_s
  end
  module_function :stringify
end

class Node
  # Return a string containing an ascii-art tree of the node's
  # structure.
  #
  def pretty_print(s = '', prefix = '')
    s << "NODE_#{self.nd_type.to_s} at #{self.nd_file}:#{self.nd_line}\n"
    self.members.each_with_index do |member, idx|
      last = (idx == self.members.size-1)
      s << "#{prefix}#{(last ? '+-' : '|-')}#{member} = "
      value = self[member]
      if Node === value then
        value.pretty_print(s, prefix + (last ? '  ' : '| '))
      elsif member == 'noex' then
        s << Noex.stringify(value) + "\n"
      else
        s << value.inspect + "\n"
      end
    end
    return s
  end
end

if __FILE__ == $0 then
  def foo # :nodoc:
    1 + 1
  end

  m = method(:foo)
  n = m.body
  puts n.pretty_print
end

