all: main1 main2

main1: libSolyanka1.so main1.c 
	gcc main1.c -L. -lSolyanka1 -o main1

main2: libSolyanka1.so libSolyanka2.so main2.c 
	gcc main2.c -ldl -o main2

libSolyanka1.so: Solyanka1.o
	gcc -shared Solyanka1.o -o libSolyanka1.so
	
libSolyanka2.so: Solyanka2.o
	gcc -shared Solyanka2.o -o libSolyanka2.so

Solyanka1.o:
	gcc -fPIC -c Solyanka1.c -o Solyanka1.o

Solyanka2.o:
	gcc -fPIC -c Solyanka2.c -o Solyanka2.o

clean:
	rm -f Solyanka1.o
	rm -f libSolyanka1.so
	rm -f Solyanka2.o
	rm -f libSolyanka2.so
	rm -f main1
	rm -f main2
