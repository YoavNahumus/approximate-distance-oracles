all: build/main.o build/ADO.o
	g++ -g build/main.o build/ADO.o -o main
	
build/main.o: main.cpp
	g++ main.cpp -g -c -o build/main.o

build/ADO.o: ADO.cpp
	g++ ADO.cpp -g -c -o build/ADO.o

clean:
	rm build/* main