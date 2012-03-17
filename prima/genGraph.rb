N=ARGV[0].to_i
SUFIX="F"
vs=(0..(N-1)).to_a
vS=vs.collect{|n| "w#{n}"}
#cs=vS.product(vS).select{|c| c[1].slice(1..-1).to_i<c[0].slice(1..-1).to_i**0.5}
cs=vS.product(vS).select{|c| c[0]<c[1]}
#cs.concat(Array.new(cs).collect{|c| c[0],c[1]=c[1],c[0]})

n1=Array.new(cs).collect{|x| [x[0], x[1], rand(15)+1]}
n2=Array.new(n1).collect{|x| x[0],x[1],x[2]=x[1],x[0],x[2]}
#ns=n1.concat(n2)
ns=n1
#puts ns.to_s
def getns n,ns
#puts "getns #{n.inspect},cs"
#puts "SELECT (#{nghs.to_s},*) FROM #{cs.to_s}"

	ns=ns.select{|v| v[0]==n}
if (ns.length>0) then
	": "+ns.collect{|x| "#{x[1]} = #{x[2]}"}.join(', ')
else
	""
end
end
width=(N**0.5+1).ceil

#vgd=File.new("#{N.to_s.rjust(4,'0')}#{SUFIX}.vgd","w+")
#vgd.puts vs.collect{|p| "#{'w'+p.to_s} (#{(p*width)*10},#{(p%width)*10}) #{getns('w'+p.to_s,ns)}"}
#vgd.close

pinp=File.new("#{N.to_s.rjust(4,'0')}#{SUFIX}.txt","w+")
pinp.puts "#{N} #{ns.length}"

pinp.puts ns.collect{|n| "#{n[0].slice(1..-1)} #{n[1].slice(1..-1)} #{n[2]}"}
pinp.close