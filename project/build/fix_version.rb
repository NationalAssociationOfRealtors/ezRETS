#!/usr/bin/env ruby
$VERBOSE = 1

require 'erb'

inversion = ARGV[0]
infile = ARGV[1]
outfile = ARGV[2]

version = inversion.split(/\./, 3)

tmpl = IO.read(File.expand_path(infile))
erb = ERB.new(tmpl, nil, "%-")
outmsg = erb.result(binding)

File.open(outfile, "w") { |file| file.print(outmsg) }
