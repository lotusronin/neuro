node.o : node.cpp node.h
	g++ -std=c++11 -c node.cpp -o node.o

parser.o : parser.cpp
	g++ -std=c++11 -c parser.cpp

tokens.o : tokens.cpp
	g++ -std=c++11 -c tokens.cpp

gencpp : parser.cpp tokens.cpp

parser.cpp : parser.y
	bison --defines=parser.h -o parser.cpp parser.y

tokens.cpp : tokens.l
	flex -o tokens.cpp tokens.l

main.o : main.cpp
	g++ -std=c++11 -c main.cpp

neuro : parser.o tokens.o node.o main.o
	g++ -std=c++11 node.o tokens.o parser.o main.o -o neuro

all : neuro

clean :
	rm neuro
