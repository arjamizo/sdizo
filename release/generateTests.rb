f=File.new('test.bat','w')
algos=['dijkstra','bellmanford']
algos=['prime', 'kruskal']
for mode in algos do
for impl in ['list', 'matr'] do
for em in [0.25, 0.5, 0.75, 1] do
(100..2000).step(100) do |v| 
#f.puts "wypenienie #{em*100}% grafu pelnego"
					f.puts "AllInPKDFB #{mode} #{impl} #{v} #{em}"
				end
			end
		end
end
f.close