#!/usr/bin/env ruby
# This ruby example requires the ruby odbc library which can be found
# at http://www.ch-werner.de/rubyodbc/  Unfortunately, this isn't available
# as a gem as of yet.

$VERBOSE = 1

require 'odbc'
require 'optparse'

DRIVER = "ezRETS ODBC Driver"
drv = ODBC::Driver.new
drv.name = DRIVER
drv.attrs["DRIVER"] = DRIVER
drv.attrs["LoginUrl"] = "http://demo.crt.realtors.org:6103/rets/login"
drv.attrs["UID"] = "Joe"
drv.attrs["PWD"] = "Schmoe"
drv.attrs["StandardNames"] = "false"

show_extra = false

usage = OptionParser.new do |opts|
  opts.banner = "Usage: GenericSearch.rb [options] <sql query>"
  opts.on('-b', '--bulk_metadata', 'Grab the metadata all at once') do |a|
    drv.attrs["UseBulkMetadata"] = "true"
  end
  opts.on('-dARG', '--debug_log ARG', 'Debug log to given file') do |a|
    drv.attrs["DebugLogFile"] = a
  end
  opts.on('-e', '--extra_info') { show_extra = true }
  opts.on('-HARG', '--http_log ARG', 'HTTP log to given file') do |a|
    drv.attrs["HttpLogFile"] = a
  end
  opts.on('-lARG', '--loginUrl ARG', 'Login URL') do |a|
    drv.attrs["LoginUrl"] = a
  end
  opts.on('-pARG', '--password ARG') { |a| drv.attrs["PWD"] = a }
  opts.on('-rARG', '--rets_version ARG') { |a| drv.attrs["RetsVersion"] = a }
  opts.on('-s', '--standard_names', 'Use standard names') do
    drv.attrs["StandardNames"] = "true"
  end
  opts.on('-uARG', '--user ARG') { |a| drv.attrs["UID"] = a }
  opts.on('-UARG', '--user_agent ARG') { |a| drv.attrs["UserAgent"] = a }
  opts.on('-h', '--help', 'Show this message') { puts opts ; exit }
  opts.parse!(ARGV)
end

if (ARGV.length < 1)
  puts usage
  exit 1
end

sql = ARGV[0]

if drv.attrs.has_key?('DebugLogFile')
  drv.attrs['UseDebugLogging'] = "true"
end

if drv.attrs.has_key?('HttpLogFile')
  drv.attrs['UseHttpLogging'] = "true"
end

dbc = ODBC::Database.new
dbc.drvconnect(drv)
dbc.run(sql) do |stmt|
  puts "Search result has #{stmt.ncols} columns"
  print "Search result has #{stmt.nrows} rows\n\n"
  if show_extra
    stmt.columns(true).each do |col|
      puts "#{col.name}: #{col.type} #{col.length}"
    end
    puts
  end
  puts stmt.columns(true).collect { |x| x.name }.join("\t")
  stmt.each { |row| puts row.join("\t") }
end
dbc.disconnect
