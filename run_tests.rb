$mini_unit_exit_code = 0

def disable_mini_unit_auto_run
  Mini::Test.class_eval do
    alias :run_ :run
    def run(*args)
      return $mini_unit_exit_code
    end
  end
end

def run_tests_with_mini_unit
  begin
    test = Mini::Test.new
    args = ARGV.dup
    args << '-v'
    $mini_unit_exit_code = test.run(args)
    exit($mini_unit_exit_code)
  ensure
    disable_mini_unit_auto_run
  end
end

def run_tests_with_test_unit
  tests = []
  ObjectSpace.each_object(Class) do |o|
    if o < Test::Unit::TestCase then
      tests << o
    end
  end

  suite = Test::Unit::TestSuite.new("RubyInternal")
  tests.each do |test|
    p test
    test.suite.tests.each do |testcase|
      suite << testcase
    end
  end

  result = Test::Unit::UI::Console::TestRunner.run(
      suite,
      verbose)
  exit(result.error_count + result.failure_count)
end

def run_tests
  begin
    require 'test/unit'
  rescue LoadError
    puts "WARNING: Test::Unit not installed; skipping tests"
    return
  end

  $:.unshift('ext')
  $:.unshift('lib')
  tests = Dir['test/test_*.rb']
  tests.each do |test|
    load test
  end

  verbose = nil
  begin
    verbose = Test::Unit::UI.const_get(:VERBOSE)
  rescue NameError
    verbose = Test::Unit::UI::Console::TestRunner.const_get(:VERBOSE)
  end

  if defined?(Mini::Test) then
    run_tests_with_mini_unit
  else
    run_tests_with_test_unit
  end
end

if __FILE__ == $0 then
  require 'timeout'
  result = nil
  timeout(600) { run_tests() }
end

