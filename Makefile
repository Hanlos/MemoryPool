memory : main.o
	g++ -o memory main.o

main.o : main.cc memory.h memory.tcc
	g++ -c main.cc

