init=2000
goal=9900
step=100
RNG=((init/step)..(goal/step)).to_a.collect!{|n| step*n}
puts RNG.collect{|n| "ruby genGraph.rb #{n}"}
puts RNG.collect{|n| "prima #{n.to_s.rjust(4,'0')}F.txt"}