all: build/graph.o build/main.o build/ADO.o
	g++ -g build/graph.o build/main.o build/ADO.o -o main

build/graph.o: graph.cpp
	g++ graph.cpp -g -c -o build/graph.o
	
build/main.o: main.cpp
	g++ main.cpp -g -c -o build/main.o

build/ADO.o: ADO.cpp
	g++ ADO.cpp -g -c -o build/ADO.o

clean:
	rm build/* main