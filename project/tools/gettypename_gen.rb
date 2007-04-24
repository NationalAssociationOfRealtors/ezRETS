#!/usr/bin/env ruby

require 'erb'
require 'optparse'

OPTIONS = {
  :prefix => "/usr/include"
}

ARGV.options do |opts|
  script_name = File.basename($0)
  opts.banner = "Usage: #{script_name} [options]"
  opts.separator ""
  opts.on("-p", "--prefix=prefix", String,
          "Prefix to find SQL header files.",
          "Default: /usr/include") { |OPTIONS[:prefix]| }
  opts.on("-h", "--help", "Show this message.") { puts opts; exit; }
  opts.parse!
end

ignore_defines = {
  "SQL_C_BOOKMARK" => 1,
  "SQL_C_VARBOOKMARK" => 1
}
  

files = %w{sql.h sqlext.h sqltypes.h}
defines = Array.new
files.each do |file|
  File.open(File.join(OPTIONS[:prefix], file)) do |f|
    f.each do |line|
      defines.push($1) if ((line =~ /^\#define\s+(SQL_C_[a-zA-Z_]+)\s+\S+/) &&
                           !ignore_defines.has_key?($1))
    end
  end
end

template = IO.read(File.join(File.dirname(__FILE__),
  "GetTypeName.r.cpp"));
erb = ERB.new(template, nil, '-')
puts erb.result(binding)
