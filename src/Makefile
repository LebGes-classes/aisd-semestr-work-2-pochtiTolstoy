all: sfml

sfml:
	g++ -O2 -std=c++20 sfml.cpp -lsfml-graphics -lsfml-window -lsfml-system -o sfml
	./sfml

stats:
	g++ -O2 -DNDEBUG -std=c++20 stats.cpp -o stats
	./stats

plot:
	python3 plot_results.py


debug:
	g++ -O0 -g -std=c++20 sfml.cpp -lsfml-graphics -lsfml-window -lsfml-system -o dbg

clean:
	rm stats sfml dbg *.csv *.png

