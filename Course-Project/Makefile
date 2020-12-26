all: drawer main

drawer: libdraw.so drawer.cpp
	g++ -g3 drawer.cpp -ldl -o drawer

libdraw.so: draw1.o
	g++ -shared draw1.o -o libdraw.so

draw1.o: draw1.cpp 
	g++ -fPIC -c draw1.cpp -o draw1.o

main: main.cpp pat
	g++ main.cpp -lpatricia -L./ -o main

pat: Trie.o Node.o 
	ar rc libpatricia.a Node.o Trie.o

Trie.o:
	g++ ./PatFuncs/Trie.cpp -c -o Trie.o 

Node.o: 
	g++ ./PatFuncs/Node.cpp -c -o Node.o

clean:
	rm -rf *.o main drawer
