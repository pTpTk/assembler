all: $(wildcard *.cpp) $(wildcard *.h)
	g++ -g $(wildcard *.cpp) -o assemble

test:
	./assemble ../C-Interpreter/test/test.s output.o

debug:
	g++ -DDEBUG -g $(wildcard *.cpp) -o assemble

ln:
	gcc -m32 output.o -o output.x