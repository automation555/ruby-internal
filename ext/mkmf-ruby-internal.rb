require 'mkmf'

ruby_source_path = arg_config('--ruby-source-path')

if ruby_source_path.nil? then
  class SetupError < StandardError; end

  # if the user did not specify the source dir, then see if we have one
  # configured
  begin
    require 'ruby_source_dir'
    # if we got here, then we successfuly loaded the configuration
  rescue SetupError 
    # otherwise, assume we are using cached files
    USING_CACHED_FILES = true
  end
else
  # the user did specify the source path
  USING_CACHED_FILES = false
  RUBY_SOURCE_DIR = ruby_source_path
end

def create_ruby_internal_makefile(name)
  rb_files = Dir['*.rb']
  rpp_files = Dir['*.rpp']
  generated_files = rpp_files.map { |f| f.sub(/\.rpp$/, '') }

  # Append the generated C files to the list of source files
  srcs = Dir['*.c']
  generated_files.each do |f|
    if f =~ /\.c$/ then
      srcs << f
    end
  end
  srcs.uniq!
  $objs = srcs.map { |f| f.sub(/\.c$/, ".#{$OBJEXT}") }

  # Turn on warnings and debugging by default on gcc
  if CONFIG['CC'] == 'gcc' then
    $CFLAGS << ' -Wall -g'
  end

  base_dir = File.dirname(__FILE__)
  $CPPFLAGS << " -I#{base_dir}"

  # Include the ruby source directory in the source if we are using 1.9
  # TODO: This means we can't build a gem for 1.9 (yet)
  ruby_version_code = RUBY_VERSION.gsub(/\./, '').to_i
  if ruby_version_code >= 190 then
    $CPPFLAGS << " -I#{RUBY_SOURCE_DIR.quote} "
  end

  create_makefile(name)

  append_to_makefile = ''

  if USING_CACHED_FILES then

    # -- Using cached files --
    rpp_files.each do |rpp_file|
      dest_file = rpp_file.sub(/\.rpp$/, '')
      append_to_makefile << <<END
#{dest_file}: cached/ruby-#{RUBY_VERSION}/#{dest_file}
  @$(RUBY) -e 'begin; require "fileutils"; rescue LoadError; require "ftools"; FileUtils = File end; FileUtils.copy("cached/ruby-#{RUBY_VERSION}/#{dest_file}", ".", :verbose => true)'
END
      end

  else
    # -- Generating files --
    rpp_files.each do |rpp_file|
    dest_file = rpp_file.sub(/\.rpp$/, '')
    append_to_makefile << <<END
#{dest_file}: #{rpp_file} #{rb_files.join(' ')}
	$(RUBY) #{base_dir}/rubypp.rb #{rpp_file} #{dest_file}
END
    end

  end

  # Dependencies
  # TODO: we could be smarter about this
  generated_headers = generated_files.select { |x| x =~ /\.h$/ }
  append_to_makefile << <<END
$(OBJS): #{generated_headers.join(' ')}
clean: clean_generated_files
clean_generated_files:
	@$(RM) #{generated_files.join(' ')}
END

  # Append it all to the makefile
  File.open('Makefile', 'a') do |makefile|
    makefile.puts(append_to_makefile)
  end

end
