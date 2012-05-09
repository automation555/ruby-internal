require 'internal/vm'

if defined?(RubyVM::InstructionSequence) then
  require 'internal/vm/bytedecoder'

  class RubyVM
    class InstructionSequence
      def as_code(indent=0)
        env = Internal::ByteDecoder::Environment.new(local_table())
        opt_pc = self.opt_pc
        self.bytedecode(env, opt_pc)
        expressions = env.expressions + env.stack
        if expressions.length == 0 then
          return nil
        elsif expressions.length == 1 and
           expressions[0].is_a?(Internal::ByteDecoder::Expression::Literal) and
           expressions[0].value == nil then
          return nil
        else
          expressions.map! { |e| "#{'  '*indent}#{e}" }
          return expressions.join("\n")
        end
      end
    end
  end
end

