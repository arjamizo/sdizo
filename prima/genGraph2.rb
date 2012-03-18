N=ARGV[0].to_i
f=File.new("#{N.to_s.rjust(4,'0')}.txt", "w+");
f.puts "#{N} #{N**2-N}"

N.times{|x|
N.times{|y|
	next if x>=y
	f.puts "#{x} #{y} #{rand(20)}"
}
}
f.close