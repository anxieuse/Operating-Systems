all: drawer main

drawer: libdraw.so drawer.cpp
	g++ -g3 drawer.cpp -ldl -o drawer

libdraw.so: draw1.o
	g++ -shared draw1.o -o libdraw.so

draw1.o: draw1.cpp 
	g++ -fPIC -c draw1.cpp -o draw1.o

main: main.cpp 
	g++ main.cpp -lpatricia -L./ -o main

clean:
	rm -rf *.o main draw
