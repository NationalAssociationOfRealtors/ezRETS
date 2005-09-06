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

# These defines are not integers, so ignore them
ignore_defines = {
  "SQL_SPEC_STRING" => 1,
  "SQL_TYPE_DRIVER_START" => 1,
  "SQL_TYPE_DRIVER_END" => 1,
  "SQL_TYPE_MIN" => 1,
  "SQL_TYPE_MAX"  => 1,
  "SQL_COLUMN_DRIVER_START" => 1,
  "SQL_CONN_OPT_MIN"  => 1,
  "SQL_CONN_OPT_MAX" => 1,
  "SQL_CONNECT_OPT_DRVR_START"  => 1,
  "SQL_OPT_TRACE_FILE_DEFAULT" => 1,
  "SQL_OPT_TRACE_FILE_DEFAULTW" => 1,
  "SQL_OPT_TRACE_FILE_DEFAULT" => 1,
  "SQL_OPT_TRACE_FILE_DEFAULTW" => 1,
  "SQL_EXT_API_LAST" => 1,
  "SQL_NUM_FUNCTIONS" => 1,
  "SQL_EXT_API_START" => 1,
  "SQL_NUM_EXTENSIONS" => 1,
  "SQL_INFO_LAST" => 1,
  "SQL_INFO_DRIVER_START" => 1,
  "SQL_FD_FETCH_RESUME" => 1,
  "SQL_TXN_VERSIONING" => 1,
  "SQL_STMT_OPT_MAX" => 1,
  "SQL_STMT_OPT_MIN" => 1,
  "SQL_FETCH_RESUME" => 1,
  "SQL_YEAR" => 1,
  "SQL_MONTH" => 1,
  "SQL_DAY" => 1,
  "SQL_HOUR" => 1,
  "SQL_MINUTE" => 1,
  "SQL_SECOND" => 1,
  "SQL_YEAR_TO_MONTH" => 1,
  "SQL_DAY_TO_HOUR" => 1,
  "SQL_DAY_TO_MINUTE" => 1,
  "SQL_DAY_TO_SECOND" => 1,
  "SQL_HOUR_TO_MINUTE" => 1,
  "SQL_HOUR_TO_SECOND" => 1,
  "SQL_MINUTE_TO_SECOND" => 1,
  "SQL_ODBC_KEYWORDS" => 1,
  "SQL_ALL_CATALOGS" => 1,
  "SQL_ALL_SCHEMAS" => 1,
  "SQL_ALL_TABLE_TYPES" => 1,
  "SQL_API" => 1,
}

files = %w{sql.h sqlext.h sqltypes.h}
defines = Array.new;
files.each do |file|
  File.open(File.join(OPTIONS[:prefix], file)) do |f|
    f.each do |line|
      defines.push($1) if ((line =~ /^\#define\s+(SQL_[a-zA-Z_]+)\s+\S+/) &&
        !ignore_defines.has_key?($1))
    end
  end
end

template = IO.read(File.join(File.dirname(__FILE__),
  "sql_defines.r.cpp"));
erb = ERB.new(template, nil, '-')
puts erb.result(binding)
