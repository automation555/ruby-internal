#
# This file is automatically generated. DO NOT MODIFY!
#
# install.rb
#
#   Copyright (c) 2000-2002 Minero Aoki <aamine@loveruby.net>
#
#   This program is free software.
#   You can distribute/modify this program under the terms of
#   the GNU Lesser General Public License version 2.
#

### begin compat.rb

unless Enumerable.instance_methods(true).include? 'inject'
module Enumerable
  def inject( result )
    each do |i|
      result = yield(result, i)
    end
    result
  end
end
end

def File.read_all( fname )
  File.open(fname, 'rb') {|f| return f.read }
end

def File.write( fname, str )
  File.open(fname, 'wb') {|f| f.write str }
end

### end compat.rb
### begin config.rb

if i = ARGV.index(/\A--rbconfig=/)
  file = $'
  ARGV.delete_at(i)
  require file
else
  require 'rbconfig'
end


class ConfigTable

  c = ::Config::CONFIG

  rubypath = c['bindir'] + '/' + c['ruby_install_name']

  major = c['MAJOR'].to_i
  minor = c['MINOR'].to_i
  teeny = c['TEENY'].to_i
  version = "#{major}.#{minor}"

  # ruby ver. >= 1.4.4?
  newpath_p = ((major >= 2) or
               ((major == 1) and
                ((minor >= 5) or
                 ((minor == 4) and (teeny >= 4)))))
  
  re = Regexp.new('\A' + Regexp.quote(c['prefix']))
  subprefix = lambda {|path|
      re === path and path.sub(re, '$prefix')
  }

  if c['rubylibdir']
    # V < 1.6.3
    stdruby    = subprefix.call(c['rubylibdir'])
    siteruby   = subprefix.call(c['sitedir'])
    versite    = subprefix.call(c['sitelibdir'])
    sodir      = subprefix.call(c['sitearchdir'])
  elsif newpath_p
    # 1.4.4 <= V <= 1.6.3
    stdruby    = "$prefix/lib/ruby/#{version}"
    siteruby   = subprefix.call(c['sitedir'])
    versite    = siteruby + '/' + version
    sodir      = "$site-ruby/#{c['arch']}"
  else
    # V < 1.4.4
    stdruby    = "$prefix/lib/ruby/#{version}"
    siteruby   = "$prefix/lib/ruby/#{version}/site_ruby"
    versite    = siteruby
    sodir      = "$site-ruby/#{c['arch']}"
  end

  DESCRIPTER = [
    [ 'prefix',    [ c['prefix'],
                     'path',
                     'path prefix of target environment' ] ],
    [ 'std-ruby',  [ stdruby,
                     'path',
                     'the directory for standard ruby libraries' ] ],
    [ 'site-ruby-common', [ siteruby,
                     'path',
                     'the directory for version-independent non-standard ruby libraries' ] ],
    [ 'site-ruby', [ versite,
                     'path',
                     'the directory for non-standard ruby libraries' ] ],
    [ 'bin-dir',   [ '$prefix/bin',
                     'path',
                     'the directory for commands' ] ],
    [ 'rb-dir',    [ '$site-ruby',
                     'path',
                     'the directory for ruby scripts' ] ],
    [ 'so-dir',    [ sodir,
                     'path',
                     'the directory for ruby extentions' ] ],
    [ 'data-dir',  [ '$prefix/share',
                     'path',
                     'the directory for shared data' ] ],
    [ 'ruby-path', [ rubypath,
                     'path',
                     'path to set to #! line' ] ],
    [ 'ruby-prog', [ rubypath,
                     'name',
                     'the ruby program using for installation' ] ],
    [ 'make-prog', [ 'make',
                     'name',
                     'the make program to compile ruby extentions' ] ],
    [ 'without-ext', [ 'no',
                       'yes/no',
                       'does not compile/install ruby extentions' ] ]
  ]

  SAVE_FILE = 'config.save'

  def ConfigTable.each_name( &block )
    keys().each(&block)
  end

  def ConfigTable.keys
    DESCRIPTER.collect {|k,*dummy| k }
  end

  def ConfigTable.each_definition( &block )
    DESCRIPTER.each(&block)
  end

  def ConfigTable.get_entry( name )
    name, ent = DESCRIPTER.assoc(name)
    ent
  end

  def ConfigTable.get_entry!( name )
    get_entry(name) or raise ArgumentError, "no such config: #{name}"
  end

  def ConfigTable.add_entry( name, vals )
    ConfigTable::DESCRIPTER.push [name,vals]
  end

  def ConfigTable.remove_entry( name )
    get_entry name or raise ArgumentError, "no such config: #{name}"
    DESCRIPTER.delete_if {|n,arr| n == name }
  end

  def ConfigTable.config_key?( name )
    get_entry(name) ? true : false
  end

  def ConfigTable.bool_config?( name )
    ent = get_entry(name) or return false
    ent[1] == 'yes/no'
  end

  def ConfigTable.value_config?( name )
    ent = get_entry(name) or return false
    ent[1] != 'yes/no'
  end

  def ConfigTable.path_config?( name )
    ent = get_entry(name) or return false
    ent[1] == 'path'
  end


  class << self

    alias newobj new

    def new
      c = newobj()
      c.__send__ :init
      c
    end

    def load
      c = newobj()
      raise InstallError, "#{File.basename $0} config first"\
                                      unless File.file? SAVE_FILE
      File.foreach(SAVE_FILE) do |line|
        k, v = line.split(/=/, 2)
        c.instance_eval {
            @table[k] = v.strip
        }
      end
      c
    end
  
  end

  def initialize
    @table = {}
  end

  def init
    DESCRIPTER.each do |k, (default, vname, desc, default2)|
      @table[k] = default
    end
  end
  private :init

  def save
    File.open(SAVE_FILE, 'w') {|f|
        @table.each do |k, v|
          f.printf "%s=%s\n", k, v if v
        end
    }
  end

  def []=( k, v )
    ConfigTable.config_key? k or raise InstallError, "unknown config option #{k}"
    if ConfigTable.path_config? k
      @table[k] = (v[0,1] != '$') ? File.expand_path(v) : v
    else
      @table[k] = v
    end
  end
    
  def []( key )
    @table[key] or return nil
    @table[key].gsub(%r<\$([^/]+)>) { self[$1] }
  end

  def set_raw( key, val )
    @table[key] = val
  end

  def get_raw( key )
    @table[key]
  end

end


class MetaConfigEnvironment

  def self.eval_file( file )
    return unless File.file? file
    new.instance_eval File.read_all(file), file, 1
  end

  private

  def config_names
    ConfigTable.keys
  end

  def config?( name )
    ConfigTable.config_key? name
  end

  def bool_config?( name )
    ConfigTable.bool_config? name
  end

  def value_config?( name )
    ConfigTable.value_config? name
  end

  def path_config?( name )
    ConfigTable.path_config? name
  end

  def add_config( name, argname, default, desc )
    ConfigTable.add_entry name,[default,argname,desc]
  end

  def add_path_config( name, default, desc )
    add_config name, 'path', default, desc
  end

  def add_bool_config( name, default, desc )
    add_config name, 'yes/no', default ? 'yes' : 'no', desc
  end

  def set_config_default( name, default )
    if bool_config? name
      ConfigTable.get_entry!(name)[0] = default ? 'yes' : 'no'
    else
      ConfigTable.get_entry!(name)[0] = default
    end
  end

  def remove_config( name )
    ent = ConfigTable.get_entry(name)
    ConfigTable.remove_entry name
    ent
  end

end

### end config.rb
### begin fileop.rb

module FileOperations

  def mkdir_p( dname, prefix = nil )
    dname = prefix + dname if prefix
    $stderr.puts "mkdir -p #{dname}" if verbose?
    return if no_harm?

    # does not check '/'... it's too abnormal case
    dirs = dname.split(%r<(?=/)>)
    if /\A[a-z]:\z/i === dirs[0]
      disk = dirs.shift
      dirs[0] = disk + dirs[0]
    end
    dirs.each_index do |idx|
      path = dirs[0..idx].join('')
      Dir.mkdir path unless dir? path
    end
  end

  def rm_f( fname )
    $stderr.puts "rm -f #{fname}" if verbose?
    return if no_harm?

    if File.exist? fname or File.symlink? fname
      File.chmod 0777, fname
      File.unlink fname
    end
  end

  def rm_rf( dn )
    $stderr.puts "rm -rf #{dn}" if verbose?
    return if no_harm?

    Dir.chdir dn
    Dir.foreach('.') do |fn|
      next if fn == '.'
      next if fn == '..'
      if dir? fn
        verbose_off {
            rm_rf fn
        }
      else
        verbose_off {
            rm_f fn
        }
      end
    end
    Dir.chdir '..'
    Dir.rmdir dn
  end

  def mv( src, dest )
    rm_f dest
    begin
      File.link src, dest
    rescue
      File.write dest, File.read_all(src)
      File.chmod File.stat(src).mode, dest
    end
    rm_f src
  end

  def install( from, dest, mode, prefix = nil )
    $stderr.puts "install #{from} #{dest}" if verbose?
    return if no_harm?

    realdest = prefix + dest if prefix
    if dir? realdest
      realdest += '/' + File.basename(from)
    end
    str = File.read_all(from)
    if diff? str, realdest
      verbose_off {
          rm_f realdest if File.exist? realdest
      }
      File.write realdest, str
      File.chmod mode, realdest

      File.open(objdir + '/InstalledFiles', 'a') {|f| f.puts realdest }
    end
  end

  def diff?( orig, targ )
    return true unless File.exist? targ
    orig != File.read_all(targ)
  end

  def command( str )
    $stderr.puts str if verbose?
    system str or raise RuntimeError, "'system #{str}' failed"
  end

  def ruby( str )
    command config('ruby-prog') + ' ' + str
  end

  def dir?( dname )
    # for corrupted windows stat()
    File.directory?((dname[-1,1] == '/') ? dname : dname + '/')
  end

  def all_files( dname )
    Dir.open(dname) {|d|
        return d.find_all {|n| File.file? "#{dname}/#{n}" }
    }
  end

  def all_dirs( dname )
    Dir.open(dname) {|d|
        return d.find_all {|n| dir? "#{dname}/#{n}" } - %w(. ..)
    }
  end

end

### end fileop.rb
### begin base.rb

class InstallError < StandardError; end


class Installer

  Version   = '3.1.3'
  Copyright = 'Copyright (c) 2000-2002 Minero Aoki'


  @toplevel = nil

  def self.declear_toplevel_installer( inst )
    @toplevel and
        raise ArgumentError, 'more than one toplevel installer decleared'
    @toplevel = inst
  end

  def self.toplevel_installer
    @toplevel
  end


  FILETYPES = %w( bin lib ext data )

  include FileOperations

  def initialize( config, opt, srcroot, objroot )
    @config = config
    @options = opt
    @srcdir = File.expand_path(srcroot)
    @objdir = File.expand_path(objroot)
    @currdir = '.'
  end

  def inspect
    "#<#{self.class} #{__id__}>"
  end

  #
  # configs/options
  #

  def get_config( key )
    @config[key]
  end

  alias config get_config

  def set_config( key, val )
    @config[key] = val
  end

  def no_harm?
    @options['no-harm']
  end

  def verbose?
    @options['verbose']
  end

  def verbose_off
    save, @options['verbose'] = @options['verbose'], false
    yield
    @options['verbose'] = save
  end

  #
  # srcdir/objdir
  #

  attr_reader :srcdir
  alias srcdir_root srcdir
  alias package_root srcdir

  def curr_srcdir
    "#{@srcdir}/#{@currdir}"
  end

  attr_reader :objdir
  alias objdir_root objdir

  def curr_objdir
    "#{@objdir}/#{@currdir}"
  end

  def srcfile( path )
    curr_srcdir + '/' + path
  end

  def srcexist?( path )
    File.exist? srcfile(path)
  end

  def srcdirectory?( path )
    dir? srcfile(path)
  end
  
  def srcfile?( path )
    File.file? srcfile(path)
  end

  def srcentries( path = '.' )
    Dir.open(curr_srcdir + '/' + path) {|d|
        return d.to_a - %w(. ..) - hookfilenames
    }
  end

  def srcfiles( path = '.' )
    srcentries(path).find_all {|fname|
        File.file? File.join(curr_srcdir, path, fname)
    }
  end

  def srcdirectories( path = '.' )
    srcentries(path).find_all {|fname|
        dir? File.join(curr_srcdir, path, fname)
    }
  end

  def dive_into( rel )
    return unless dir? "#{@srcdir}/#{rel}"

    dir = File.basename(rel)
    Dir.mkdir dir unless dir? dir
    save = Dir.pwd
    Dir.chdir dir
    $stderr.puts '---> ' + rel if verbose?
    @currdir = rel
    yield
    Dir.chdir save
    $stderr.puts '<--- ' + rel if verbose?
    @currdir = File.dirname(rel)
  end

  #
  # config
  #

  def exec_config
    exec_task_traverse 'config'
  end

  def config_dir_bin( rel )
  end

  def config_dir_lib( rel )
  end

  def config_dir_ext( rel )
    extconf if extdir? curr_srcdir
  end

  def extconf
    opt = @options['config-opt'].join(' ')
    command "#{config('ruby-prog')} #{curr_srcdir}/extconf.rb #{opt}"
  end

  def config_dir_data( rel )
  end

  #
  # setup
  #

  def exec_setup
    exec_task_traverse 'setup'
  end

  def setup_dir_bin( relpath )
    all_files(curr_srcdir).each do |fname|
      add_rubypath "#{curr_srcdir}/#{fname}"
    end
  end

  SHEBANG_RE = /\A\#!\s*\S*ruby\S*/

  def add_rubypath( path )
    $stderr.puts %Q<set #! line to "\#!#{config('ruby-path')}" for #{path} ...> if verbose?
    return if no_harm?

    tmpfile = File.basename(path) + '.tmp'
    begin
      File.open(path) {|r|
      File.open(tmpfile, 'w') {|w|
          first = r.gets
          return unless SHEBANG_RE === first   # reject '/usr/bin/env ruby'

          w.print first.sub(SHEBANG_RE, '#!' + config('ruby-path'))
          w.write r.read
      } }
      mv tmpfile, File.basename(path)
    ensure
      rm_f tmpfile if File.exist? tmpfile
    end
  end

  def setup_dir_lib( relpath )
  end

  def setup_dir_ext( relpath )
    make if extdir?(curr_srcdir)
  end

  def make
    command config('make-prog')
  end

  def setup_dir_data( relpath )
  end

  #
  # install
  #

  def exec_install
    exec_task_traverse 'install'
  end

  def install_dir_bin( rel )
    install_files targfiles, config('bin-dir') + '/' + rel, 0755
  end

  def install_dir_lib( rel )
    install_files targfiles, config('rb-dir') + '/' + rel, 0644
  end

  def install_dir_ext( rel )
    install_dir_ext_main File.dirname(rel) if extdir?(curr_srcdir)
  end

  def install_dir_ext_main( rel )
    install_files allext('.'), config('so-dir') + '/' + rel, 0555
  end

  def install_dir_data( rel )
    install_files targfiles, config('data-dir') + '/' + rel, 0644
  end

  def install_files( list, dest, mode )
    mkdir_p dest, @options['install-prefix']
    list.each do |fname|
      install fname, dest, mode, @options['install-prefix']
    end
  end
  
  def targfiles
    (targfilenames() - hookfilenames()).collect {|fname|
        File.exist?(fname) ? fname : File.join(curr_srcdir(), fname)
    }
  end

  def targfilenames
    [ curr_srcdir(), '.' ].inject([]) {|ret, dir|
        ret | all_files(dir)
    }
  end

  def hookfilenames
    %w( pre-%s post-%s pre-%s.rb post-%s.rb ).collect {|fmt|
        %w( config setup install clean ).collect {|t| sprintf fmt, t }
    }.flatten
  end

  def allext( dir )
    _allext(dir) or raise InstallError,
        "no extention exists: Have you done 'ruby #{$0} setup' ?"
  end

  DLEXT = /\.#{ ::Config::CONFIG['DLEXT'] }\z/

  def _allext( dir )
    Dir.open(dir) {|d|
        return d.find_all {|fname| DLEXT === fname }
    }
  end

  #
  # clean
  #

  def exec_clean
    exec_task_traverse 'clean'
    rm_f 'config.save'
    rm_f 'InstalledFiles'
  end

  def clean_dir_bin( rel )
  end

  def clean_dir_lib( rel )
  end

  def clean_dir_ext( rel )
    clean
  end
  
  def clean
    command config('make-prog') + ' clean' if File.file? 'Makefile'
  end

  def clean_dir_data( rel )
  end

  #
  # lib
  #

  def exec_task_traverse( task )
    run_hook 'pre-' + task
    FILETYPES.each do |type|
      if config('without-ext') == 'yes' and type == 'ext'
        $stderr.puts 'skipping ext/* by user option' if verbose?
        next
      end
      traverse task, type, task + '_dir_' + type
    end
    run_hook 'post-' + task
  end

  def traverse( task, rel, mid )
    dive_into(rel) {
        run_hook 'pre-' + task
        __send__ mid, rel.sub(%r_\A.*?(?:/|\z)_, '')
        all_dirs(curr_srcdir).each do |d|
          traverse task, rel + '/' + d, mid
        end
        run_hook 'post-' + task
    }
  end

  def run_hook( name )
    try_run_hook curr_srcdir + '/' + name           or
    try_run_hook curr_srcdir + '/' + name + '.rb'
  end

  def try_run_hook( fname )
    return false unless File.file? fname

    env = self.dup
    begin
      env.instance_eval File.read_all(fname), fname, 1
    rescue
      raise InstallError, "hook #{fname} failed:\n" + $!.message
    end
    true
  end

  def extdir?( dir )
    File.exist? dir + '/MANIFEST'
  end

end

### end base.rb
### begin toplevel.rb

class ToplevelInstaller < Installer

  TASKS = [
    [ 'config',   'saves your configurations' ],
    [ 'show',     'shows current configuration' ],
    [ 'setup',    'compiles extention or else' ],
    [ 'install',  'installs files' ],
    [ 'clean',    "does `make clean' for each extention" ]
  ]


  def initialize( root )
    super nil, {'verbose' => true}, root, '.'
    Installer.declear_toplevel_installer self
  end


  def execute
    run_metaconfigs

    case task = parsearg_global()
    when 'config'
      @config = ConfigTable.new
    else
      @config = ConfigTable.load
    end
    parsearg_TASK task

    exectask task
  end


  def run_metaconfigs
    MetaConfigEnvironment.eval_file "#{srcdir_root()}/#{metaconfig()}"
  end

  def metaconfig
    'metaconfig'
  end


  def exectask( task )
    if task == 'show'
      exec_show
    else
      try task
    end
  end

  def try( task )
    $stderr.printf "#{File.basename $0}: entering %s phase...\n", task if verbose?
    begin
      __send__ 'exec_' + task
    rescue
      $stderr.printf "%s failed\n", task
      raise
    end
    $stderr.printf "#{File.basename $0}: %s done.\n", task if verbose?
  end

  #
  # processing arguments
  #

  def parsearg_global
    task_re = /\A(?:#{TASKS.collect {|i| i[0] }.join '|'})\z/

    while arg = ARGV.shift
      case arg
      when /\A\w+\z/
        task_re === arg or raise InstallError, "wrong task: #{arg}"
        return arg

      when '-q', '--quiet'
        @options['verbose'] = false

      when       '--verbose'
        @options['verbose'] = true

      when '-h', '--help'
        print_usage $stdout
        exit 0

      when '-v', '--version'
        puts "#{File.basename $0} version #{Version}"
        exit 0
      
      when '--copyright'
        puts Copyright
        exit 0

      else
        raise InstallError, "unknown global option '#{arg}'"
      end
    end

    raise InstallError, "No task or global option given.
Typical installation procedure is:
    $ ruby #{File.basename $0} config
    $ ruby #{File.basename $0} setup
    # ruby #{File.basename $0} install  (may require root privilege)
"
  end


  def parsearg_TASK( task )
    mid = "parsearg_#{task}"
    if respond_to? mid, true
      __send__ mid
    else
      ARGV.empty? or
          raise InstallError, "#{task}:  unknown options: #{ARGV.join ' '}"
    end
  end

  def parsearg_config
    re = /\A--(#{ConfigTable.keys.join '|'})(?:=(.*))?\z/
    @options['config-opt'] = []

    while i = ARGV.shift
      if /\A--?\z/ === i
        @options['config-opt'] = ARGV.dup
        break
      end
      m = re.match(i) or raise InstallError, "config: unknown option #{i}"
      name, value = m.to_a[1,2]
      if value
        if ConfigTable.bool_config?(name)
          /\A(y(es)?|n(o)?|t(rue)?|f(alse))\z/i === value or raise InstallError, "config: --#{name} allows only yes/no for argument"
          value = (/\Ay(es)?|\At(rue)/i === value) ? 'yes' : 'no'
        end
      else
        ConfigTable.bool_config?(name) or raise InstallError, "config: --#{name} requires argument"
        value = 'yes'
      end
      @config[name] = value
    end
  end

  def parsearg_install
    @options['no-harm'] = false
    @options['install-prefix'] = ''
    while a = ARGV.shift
      case a
      when /\A--no-harm\z/
        @options['no-harm'] = true
      when /\A--prefix=(.*)\z/
        path = $1
        path = File.expand_path(path) unless path[0,1] == '/'
        @options['install-prefix'] = path
      else
        raise InstallError, "install: unknown option #{a}"
      end
    end
  end


  def print_usage( out )
    out.puts 'Typical Installation Procedure:'
    out.puts "  $ ruby #{File.basename $0} config"
    out.puts "  $ ruby #{File.basename $0} setup"
    out.puts "  # ruby #{File.basename $0} install (may require root privilege)"
    out.puts
    out.puts 'Detailed Usage:'
    out.puts "  ruby #{File.basename $0} <global option>"
    out.puts "  ruby #{File.basename $0} [<global options>] <task> [<task options>]"

    fmt = "  %-20s %s\n"
    out.puts
    out.puts 'Global options:'
    out.printf fmt, '-q,--quiet',   'suppress message outputs'
    out.printf fmt, '   --verbose', 'output messages verbosely'
    out.printf fmt, '-h,--help',    'print this message'
    out.printf fmt, '-v,--version', 'print version and quit'
    out.printf fmt, '   --copyright',  'print copyright and quit'

    out.puts
    out.puts 'Tasks:'
    TASKS.each do |name, desc|
      out.printf "  %-10s  %s\n", name, desc
    end

    out.puts
    out.puts 'Options for config:'
    ConfigTable.each_definition do |name, (default, arg, desc, default2)|
      out.printf "  %-20s %s [%s]\n",
                 '--'+ name + (ConfigTable.bool_config?(name) ? '' : '='+arg),
                 desc,
                 default2 || default
    end
    out.printf "  %-20s %s [%s]\n",
        '--rbconfig=path', 'your rbconfig.rb to load', "running ruby's"

    out.puts
    out.puts 'Options for install:'
    out.printf "  %-20s %s [%s]\n",
        '--no-harm', 'only display what to do if given', 'off'
    out.printf "  %-20s %s [%s]\n",
        '--prefix',  'install path prefix', '$prefix'

    out.puts
  end

  #
  # config
  #

  def exec_config
    super
    @config.save
  end

  #
  # show
  #

  def exec_show
    ConfigTable.each_name do |k|
      v = @config.get_raw(k)
      if not v or v.empty?
        v = '(not specified)'
      end
      printf "%-10s %s\n", k, v
    end
  end

end

### end toplevel.rb

if $0 == __FILE__
  begin
    installer = ToplevelInstaller.new(File.dirname($0))
    installer.execute
  rescue
    raise if $DEBUG
    $stderr.puts $!.message
    $stderr.puts "Try 'ruby #{$0} --help' for detailed usage."
    exit 1
  end
end
