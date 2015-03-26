#!/usr/bin/ruby

# List directory contents.

path = ARGV[0] == nil ? "." : ARGV[0].dup

# Remove trailing slashes
while path =~ /\/$/
	path.chop!
end

entries = Dir.open(path)
for entry in entries
	unless entry == "." || entry == ".."
		filePath	= "#{path}/#{entry}"
		fileStat = File.stat(filePath)
		if fileStat.directory?
			puts "dir : #{filePath}"
		elsif fileStat.file?
			puts "file: #{filePath}"
		end
	end
end
entries.close()
