f=File.new('test.bat','w')
for mode in ['dijkstra'] do
for em in [0.25, 0.5, 0.75, 1] do
for impl in ['list', 'matr'] do
(1000..2000).step(100) do |v| 
#f.puts "wypenienie #{em*100}% grafu pelnego"
					f.puts "AllInPKDFB #{mode} #{impl} #{v} #{em}"
				end
			end
		end
end
f.close