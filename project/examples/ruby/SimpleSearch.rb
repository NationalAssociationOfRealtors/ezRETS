#!/usr/bin/env ruby
#
# This ruby example requires the ruby odbc library which can be found
# at http://www.ch-werner.de/rubyodbc/  Unfortunately, this isn't available
# as a gem as of yet.

require 'odbc'

ODBC::connect("retstest", "Joe", "Schmoe") do |dbc|
  statement = "SELECT ListingID,ListDate" +
    "  FROM data:Property:ResidentialProperty" +
    " WHERE ListPrice > 0"

  dbc.run(statement) do |stmt|
    puts "Search result has #{stmt.ncols} columns"
    print "Search Result has #{stmt.nrows} rows\n\n"
    colno = 1
    stmt.columns do |col|
      puts "col[#{colno}]: #{col.name} #{col.type} #{col.length}"
      colno += 1
    end
    puts
    stmt.each { |row| puts row.join(" ") }
  end
end  
