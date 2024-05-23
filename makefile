all: build/graph.o build/main.o
	g++ -g graph.o main.o -o main

build/graph.o: graph.cpp
	g++ graph.cpp -g -c -o build/graph.o
	
build/main.o: main.cpp
	g++ main.cpp -g -c -o build/main.o

clean:
	rm build/* main