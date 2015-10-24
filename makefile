parser.cpp : parser.y
	bison --defines=parser.h -o parser.cpp parser.y

tokens.cpp : tokens.l
	flex -o tokens.cpp tokens.l

neuro : parser.cpp tokens.cpp
	g++ -std=c++11 tokens.cpp parser.cpp main.cpp -o neuro

all : neuro

clean :
	rm neuro
